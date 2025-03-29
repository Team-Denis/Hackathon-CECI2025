#include "PhysicalStorage/QRCodeStorage.hpp"
#include <filesystem>

namespace PhysicalStorage {
    // Creates a QR code from binary data and writes it to a file
    bool QRCodeStorage::dataToQRFile(const std::vector<uint8_t> &data, const std::string &filename,
                                     QRecLevel errorCorrectionLevel) {
        if (data.empty()) {
            std::cerr << "Cannot create QR code: Empty data" << std::endl;
            return false;
        }

        // Create a QR code
        QRcode *qrCode = QRcode_encodeData(data.size(), data.data(), 0, errorCorrectionLevel);
        if (!qrCode) {
            std::cerr << "Failed to generate QR code" << std::endl;
            return false;
        }

        // Write to file
        bool success = writeQRToFile(*qrCode, filename);

        // Free memory
        QRcode_free(qrCode);

        return success;
    }

    // Reads a QR code from a file and decodes it
    std::vector<uint8_t> QRCodeStorage::qrFileToData(const std::string &filename) {
        // Create a zbar image scanner
        zbar_image_scanner_t *scanner = zbar_image_scanner_create();
        zbar_image_scanner_set_config(scanner, 0, ZBAR_CFG_ENABLE, 1);

        // Open PBM file
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Cannot open file for reading: " << filename << std::endl;
            return {};
        }

        // Read PBM header
        std::string magic;
        int width, height;
        file >> magic >> width >> height;

        // Read QR code data
        uint8_t image[width * height];
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int pixel;
                fscanf(file, "%d", &pixel);
                image[i * width + j] = pixel ? 255 : 0;
            }
        }

        file.close();

        // Create a zbar image
        zbar_image_t *zbarImage = zbar_image_create();
        zbar_image_set_format(zbarImage, *(int*)"Y800");
        zbar_image_set_size(zbarImage, width, height);
        zbar_image_set_data(zbarImage, image, width * height, zbar_image_free_data);

        // Scan the image
        zbar_scan_image(scanner, zbarImage);

        // Extract data from the first symbol
        zbar_symbol_t *symbol = zbar_image_first_symbol(zbarImage);
        std::vector<uint8_t> data;
        
        while (symbol) {
            if (zbar_symbol_get_type(symbol) == ZBAR_QRCODE) {
                const char *data = zbar_symbol_get_data(symbol);
                size_t dataSize = zbar_symbol_get_data_length(symbol);
                for (int i = 0; i < dataSize; i++) {
                    data.push_back(data[i]);
                }
            }

            symbol = zbar_symbol_next(symbol);
        }

        // Clean up
        zbar_image_destroy(zbarImage);
        zbar_image_scanner_destroy(scanner);

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

    // Helper to write QR code to PBM file
    bool QRCodeStorage::writeQRToFile(const QRcode &qr, const std::string &filename) {
        std::ofstream file(filename);
        if (!file) {
            std::cerr << "Cannot open file for writing: " << filename << std::endl;
            return false;
        }

        // Write PBM header (P1 format)
        file << "P1" << std::endl;
        file << qr.width << " " << qr.width << std::endl;

        // Write QR code data
        for (int i = 0; i < qr.width; i++) {
            for (int j = 0; j < qr.width; j++) {
                // 0 is black, 1 is white in PBM format (invert QR code bit)
                unsigned char byte = qr.data[i * qr.width + j];
                bool bit = (byte & 1) != 0;
                file << (bit ? "0 " : "1 ");
            }
            file << std::endl;
        }

        file.close();
        return true;
    }
} // namespace PhysicalStorage
