#include "PhysicalStorage/QRCodeStorage.hpp"
#include "PhysicalStorage/PBMUtils.h"
#include <bitset>


namespace PhysicalStorage {
    // Creates a QR code from binary data and writes it to a file
    bool QRCodeStorage::dataToQRFile(const std::vector<uint8_t> &data, const std::string &filename) {
        if (data.empty()) {
            std::cerr << "Cannot create QR code: Empty data" << std::endl;
            return false;
        }

        uint64_t dataSize = data.size();

        std::vector<uint8_t> imageData;

        // Write data to image
        for (uint64_t i = 0; i < dataSize; i++) {
            uint8_t byte = data[i];
            std::bitset<8> bits(byte);
            std::cout << bits << std::endl;
            for (int j = 0; j < 8; j++) {
                uint8_t bit = byte & 0b10000000;
                byte = byte << 1;
                imageData.push_back(bit ? 0 : 255);
            }
        }

        // Write image to file
        int result = stbi_write_png(filename.c_str(), 32, dataSize/4, 1, imageData.data(), 0);

        return result != 0;
    }

    // Reads a QR code from a file and decodes it
    std::vector<uint8_t> QRCodeStorage::qrFileToData(const std::string &filename) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Cannot open PBM file: " << filename << std::endl;
            return {};
        }

        // use stb_image to read the image
        int width, height, channels;
        uint8_t *imageData = stbi_load(filename.c_str(), &width, &height, &channels, 0);

        if (!imageData) {
            std::cerr << "Failed to read image data" << std::endl;
            return {};
        }

        std::vector<uint8_t> data;

        // Print width and height
        std::cout << "Width: " << width << ", Height: " << height << std::endl;

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

        return data;
    }

    // Process data in chunks, creating a QR code for each chunk
    int QRCodeStorage::processDataChunks(const std::vector<uint8_t> &data, size_t chunkSize,
                                         const std::string &outputDir, const std::string &baseFilename) {
        if (data.empty() || chunkSize == 0) {
            return 0;
        }

        // Create output directory if it doesn't exist
        if (!std::filesystem::exists(outputDir)) {
            std::filesystem::create_directories(outputDir);
        }

        // Calculate number of chunks
        size_t numChunks = (data.size() + chunkSize - 1) / chunkSize;

        // Process each chunk
        int successCount = 0;
        for (size_t i = 0; i < numChunks; i++) {
            // Calculate chunk boundaries
            size_t startPos = i * chunkSize;
            size_t endPos = std::min(startPos + chunkSize, data.size());
            size_t actualChunkSize = endPos - startPos;

            // Extract chunk data
            std::vector<uint8_t> chunkData(data.begin() + startPos, data.begin() + endPos);

            // Create QR code filename
            std::string qrFilename = outputDir + "/" + baseFilename + "_chunk_" + std::to_string(i) +
                                     Constants::QR_CODE_EXT;

            // Generate QR code
            if (dataToQRFile(chunkData, qrFilename)) {
                successCount++;
            }
        }

        return successCount;
    }

} // namespace PhysicalStorage
