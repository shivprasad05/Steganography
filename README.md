# C++ Steganography Tool 🖼️

A simple, fast, and secure command-line tool for hiding secret text messages inside PNG images using LSB steganography. This project is written in modern C++ with an Object-Oriented approach and has no external dependencies.

A command-line interface showing the steganography tool in action.

---

## ✨ Features

* **Embed Secret Messages**: Hide any text message within the pixels of a PNG image.
* **Extract Secret Messages**: Reveal the hidden message from a stego-image.
* **Password Protection**: Your message is encrypted with a password using a simple XOR cipher, making it unreadable without the key.
* **Imperceptible**: The output image is visually identical to the original.
* **Lightweight**: Uses the brilliant single-header `stb_image` libraries, so there are no complex dependencies to install.
* **Cross-Platform**: Written in standard C++, it can be compiled and run on Windows, macOS, and Linux.

---

## 🤔 How It Works

This tool employs the **Least Significant Bit (LSB) steganography** technique.

1.  **Pixels**: Every pixel in an image has color values (Red, Green, Blue). Each value is a byte (8 bits).
2.  **Hiding Bits**: The LSB is the rightmost bit of a byte (e.g., in `1101011`**`1`**). Changing this bit causes a tiny, imperceptible change to the color.
3.  **The Process**: The tool converts your secret message into a stream of bits. It then overwrites the LSB of each color value in the image with the bits from your message.
4.  **Security**: Before embedding, the message (plus a special end-of-message marker) is encrypted with your password. This ensures that even if someone knows there's a message, they can't read it without the password.

---

## 🚀 Getting Started

Follow these steps to get the project compiled and running on your local machine.

### Prerequisites

You need a C++ compiler.
* **Windows**: Install MinGW-w64 (via MSYS2 is recommended).
* **macOS**: Install Xcode Command Line Tools (`xcode-select --install`).
* **Linux**: Install `build-essential` or `g++` (`sudo apt-get install build-essential`).

### Setup

1.  **Clone the repository:**
    ```sh
    git clone [https://github.com/your-username/your-repo.git](https://github.com/your-username/your-repo.git)
    cd your-repo
    ```

2.  **Download required headers:**
    This project uses the `stb` single-file public domain libraries. You need two files:
    * [stb_image.h](https://raw.githubusercontent.com/nothings/stb/master/stb_image.h)
    * [stb_image_write.h](https://raw.githubusercontent.com/nothings/stb/master/stb_image_write.h)

    Download them and place them in the same directory as `main.cpp`. Your project folder should look like this:
    ```
    /your-repo
    |-- main.cpp
    |-- stb_image.h
    |-- stb_image_write.h
    ```

---

## 💻 Compilation & Usage

### Compilation

Open your terminal or command prompt, navigate to the project directory, and run the following command:

```sh
g++ main.cpp -o steg_tool -std=c++17 -O2
