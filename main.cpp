#include <iostream>
#include <string> // strings
#include <vector>
#include <stdexcept>

// IMPORTANT: Define these before including the stb headers in ONE .cpp file
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

class Steganography {
private:
    unsigned char* imageData = nullptr;
    int width, height, channels;
    long imageSize;
    // A unique string to mark the end of our secret message
    const std::string endOfMessageDelimiter = "::STEG_EOF::";

    /**
     * @brief A simple XOR encryption/decryption function.
     * @param text The string to be encrypted or decrypted.
     * @param key The password.
     * @return The processed string.
     */
    std::string encryptDecrypt(const std::string& text, const std::string& key) {
        std::string output = text;
        for (size_t i = 0; i < text.size(); ++i) {
            output[i] = text[i] ^ key[i % key.length()];
        }
        return output;
    }

    /**
     * @brief Sets the least significant bit of a byte.
     * @param byte The byte to modify.
     * @param bit The bit ('0' or '1') to set.
     */
    void setLSB(unsigned char& byte, char bit) {
        if (bit == '1') {
            byte |= 1;  // Set LSB to 1 (e.g., 11111110 | 00000001 = 11111111)
        } else {
            byte &= ~1; // Set LSB to 0 (e.g., 11111111 & 11111110 = 11111110)
        }
    }

    /**
     * @brief Gets the least significant bit of a byte.
     * @param byte The byte to read from.
     * @return '0' or '1'.
     */
    char getLSB(unsigned char byte) {
        return (byte & 1) ? '1' : '0';
    }

public:
    // Constructor: Loads the image data
    Steganography(const std::string& imagePath) {
        imageData = stbi_load(imagePath.c_str(), &width, &height, &channels, 0);
        if (imageData == nullptr) {
            throw std::runtime_error("Error: Could not load image at " + imagePath);
        }
        imageSize = width * height * channels;
        std::cout << "Image loaded successfully: " << width << "x" << height << ", " << channels << " channels." << std::endl;
    }

    // Destructor: Frees the image data
    ~Steganography() {
        if (imageData) {
            stbi_image_free(imageData);
        }
    }

    /**
     * @brief Embeds a secret message into the loaded image.
     * @param message The secret message string.
     * @param password The password to encrypt the message.
     * @param outputPath The path to save the new image.
     */
    void embedMessage(const std::string& message, const std::string& password, const std::string& outputPath) {
        // 1. Prepare the message
        std::string encryptedMessage = encryptDecrypt(message + endOfMessageDelimiter, password);
        std::string messageInBinary;
        for (char c : encryptedMessage) {
            // Convert each character to an 8-bit binary string
            for (int i = 7; i >= 0; --i) {
                messageInBinary += ((c >> i) & 1) ? '1' : '0';
            }
        }

        // 2. Check if the image has enough capacity
        if (messageInBinary.length() > imageSize) {
            throw std::runtime_error("Error: Image is too small to hold the message.");
        }

        // 3. Embed the bits into the image data
        for (size_t i = 0; i < messageInBinary.length(); ++i) {
            setLSB(imageData[i], messageInBinary[i]);
        }

        // 4. Save the new image
        // stbi_write_png supports .png, .bmp, .tga, .jpg. PNG is recommended as it's lossless.
        if (stbi_write_png(outputPath.c_str(), width, height, channels, imageData, width * channels) == 0) {
            throw std::runtime_error("Error: Could not save the image to " + outputPath);
        }
        std::cout << "Message embedded successfully. Output saved to: " << outputPath << std::endl;
    }

    /**
     * @brief Extracts a secret message from the loaded image.
     * @param password The password to decrypt the message.
     * @return The hidden message.
     */
    std::string extractMessage(const std::string& password) {
        std::string extractedBinary;
        std::string plaintextMessage; // This will hold the decrypted message as we build it
        char extractedChar = 0;
        long messageCharIndex = 0; // To keep track of our position for the password XOR

        // Extract LSBs from each byte of image data
        for (long i = 0; i < imageSize; ++i) {
            char lsb = getLSB(imageData[i]);
            extractedBinary += lsb;

            // When we have 8 bits, convert them to a character
            if (extractedBinary.length() == 8) {
                extractedChar = 0; // Reset the character byte
                // Convert the 8-bit binary string to a char
                for (int j = 0; j < 8; ++j) {
                    if (extractedBinary[j] == '1') {
                        extractedChar |= (1 << (7 - j));
                    }
                }
                
                // *** THE FIX IS HERE ***
                // Decrypt the single character immediately using the password
                char decryptedChar = extractedChar ^ password[messageCharIndex % password.length()];
                plaintextMessage += decryptedChar;
                messageCharIndex++;
                
                // Reset binary string for the next character
                extractedBinary = "";

                // Now, check for the PLAINTEXT delimiter in the decrypted stream
                if (plaintextMessage.length() >= endOfMessageDelimiter.length() &&
                    plaintextMessage.substr(plaintextMessage.length() - endOfMessageDelimiter.length()) == endOfMessageDelimiter) {
                    
                    // Delimiter found! Return the final message, stripping the delimiter off the end.
                    return plaintextMessage.substr(0, plaintextMessage.length() - endOfMessageDelimiter.length());
                }
            }
        }
        // If the loop finishes, the delimiter was never found.
        throw std::runtime_error("Error: No message found or delimiter is missing.");
    }
};
void printUsage() {
    std::cout << "Usage:" << std::endl;
    std::cout << "  To embed:  main.exe embed <input_image.png> <output_image.png> \"<secret_message>\" <password>" << std::endl;
    std::cout << "  To extract: main.exe extract <stego_image.png> <password>" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        printUsage();
        return 1;
    }

    try {
        std::string mode = argv[1];
        if (mode == "embed" && argc == 6) {
            std::string inputPath = argv[2];
            std::string outputPath = argv[3];
            std::string message = argv[4];
            std::string password = argv[5];

            Steganography steg(inputPath);
            steg.embedMessage(message, password, outputPath);

        } else if (mode == "extract" && argc == 4) {
            std::string inputPath = argv[2];
            std::string password = argv[3];

            Steganography steg(inputPath);
            std::string secretMessage = steg.extractMessage(password);
            std::cout << "-------------------------" << std::endl;
            std::cout << "Extracted Secret Message:" << std::endl;
            std::cout << secretMessage << std::endl;
            std::cout << "-------------------------" << std::endl;

        } else {
            printUsage();
            return 1;
        }
    } catch (const std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}