# Hackathon-CECI2025

Hackathon-CECI2025 is an innovative project that combines **cryptography**, **data compression**, and **visualization** by leveraging the **Game of Life** to generate QR codes containing encoded data. This project explores a unique method to encode and store information visually and securely.

---

## Features

- **Data Encoding**: Converts files into a compressed binary format and encodes them into QR codes.
- **Data Decoding**: Reads the generated QR codes and reconstructs the original files.
- **File Chunking**: Splits files into 256x256 bit matrices for optimized processing.
- **Visualization**: Displays QR codes or bit matrices using a graphical interface.
- **Game of Life**: Generates dynamic patterns inspired by the famous cellular automaton to enrich QR codes.

---

## Installation

### Prerequisites

- **CMake** (version 3.10 or higher)
- **C++ Compiler** (supporting C++17)
- **Dependencies**:
  - [OpenSSL](https://www.openssl.org/) (for MD5 hashing)
  - [libqrencode](https://fukuchi.org/works/qrencode/) (for QR code generation)
  - [raylib](https://www.raylib.com/) (for graphical visualization)
