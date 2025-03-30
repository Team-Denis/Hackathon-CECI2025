#include "PhysicalStorage/QRCodeStorage.hpp"
#include "PhysicalStorage/PBMUtils.h"
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

        uint64_t dataSize = data.size();

        std::vector<uint8_t> imageData;

        // Write data to image
        for (uint64_t i = 0; i < dataSize; i++) {
            uint8_t byte = data[i];
            for (int j = 0; j < 8; j++) {
                uint8_t bit = byte & 0b10000000;
                byte = byte << 1;
                imageData.push_back(bit ? 0 : 255);
            }
        }

        // Write image to file
        int result = stbi_write_png(out.c_str(), 32, dataSize/4, 1, imageData.data(), 0);

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
        uint8_t byte = 0;
        for (int i = 0; i < width * height; i++) {
            byte = byte << 1;
            byte |= imageData[i] == 0 ? 1 : 0;

            if (i % 8 == 7) {
                data.push_back(byte);
                byte = 0;
            }
        }

        stbi_image_free(imageData);


        // Write data to file
        std::ofstream outFile(out, std::ios::binary);
        if (!outFile) {
            std::cerr << "Cannot open output file: " << out << std::endl;
            return {};
        }

        for (uint8_t byte : data) {
            outFile.write(reinterpret_cast<const char *>(&byte), sizeof(byte));
        }

        return true;
    }

} // namespace PhysicalStorage
