#include "PhysicalStorage/QRCodeStorage.hpp"
#include "PhysicalStorage/HammingCode.h"
#include <bitset>


namespace PhysicalStorage {

    // Creates a QR code from binary data and writes it to a file
    bool QRCodeStorage::fileToQR(const std::string &in, const std::string &out) {

        // Read data from file
        std::vector<uint8_t> data;

        std::ifstream file(in, std::ios::binary);
        if (!file) {
            std::cerr << "Cannot open file: " << in << std::endl;
            return false;
        }

        // Read data from file
        uint8_t byte;
        while (file.read(reinterpret_cast<char *>(&byte), sizeof(byte))) {
            data.push_back(byte);
        }

        if (data.empty()) {
            std::cerr << "Cannot create QR code: Empty data" << std::endl;
            return false;
        }

        std::vector<std::vector<bool>> encodedData;
        for (uint8_t byte : data) {
            encodedData.push_back(HammingCode::encodeByte(byte));
        }

        uint64_t dataSize = data.size();
        uint64_t bits_size = encodedData[0].size();

        std::vector<uint8_t> imageData;

        // Write data to image
        for (uint64_t i = 0; i < dataSize; i++) {
            std::vector<bool> bits = encodedData[i];
            for (int j = 0; j < bits.size(); j++) {
                imageData.push_back(bits[j] ? 0 : 255);
            }
        }

        // Write image to file
        int result = stbi_write_png(out.c_str(), bits_size, dataSize, 1, imageData.data(), bits_size);

        return result != 0;
    }

    // Reads a QR code from a file and decodes it
    bool QRCodeStorage::QRToFile(const std::string &in, const std::string &out) {
        std::ifstream file(in);
        if (!file) {
            std::cerr << "Cannot open QR code file: " << in << std::endl;
            return {};
        }

        // use stb_image to read the image
        int width, height, channels;
        uint8_t *imageData = stbi_load(in.c_str(), &width, &height, &channels, 0);

        if (!imageData) {
            std::cerr << "Failed to read image data" << std::endl;
            return {};
        }

        std::vector<uint8_t> data;

        // Read image data
        for (int y = 0; y < height; y++) {
            std::vector<bool> bits;
            for (int x = 0; x < width; x++) {
                bits.push_back(imageData[y * width + x] == 0);
            }

            uint8_t byte = HammingCode::decodeByte(bits);
            std::cout << "Decoded byte: " << std::bitset<8>(byte) << std::endl;
            data.push_back(byte);
        }

        // Write data to file
        std::ofstream outFile(out, std::ios::binary);
        if (!outFile) {
            std::cerr << "Cannot open file: " << out << std::endl;
            return false;
        }

        for (uint8_t byte : data) {
            outFile.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
        }

        return true;
    }

} // namespace PhysicalStorage