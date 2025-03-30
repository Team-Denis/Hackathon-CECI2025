/*
* This data storage is inspired by the GitHub Arctic Vault : 
* https://archiveprogram.github.com/arctic-vault/
*/

#ifndef QR_CODE_STORAGE_HPP
#define QR_CODE_STORAGE_HPP

#include "PhysicalStorage/StorageCommon.hpp"
#include "stb_image.h"
#include "stb_image_write.h"
#include <string>
#include <vector>
#include <filesystem>

const uint64_t CHUNK_SIZE = 256*256;

namespace PhysicalStorage {
    /**
     * Handles conversion between binary data and QR codes for archival storage
     */
    class QRCodeStorage {
    public:
        /**
         * Creates a QR code from binary data and writes it to a file in PNG format.
         *
         * @param data Binary data to encode
         * @param filename Output filename for the QR code
         * @param errorCorrectionLevel QR code error correction level (default: QR_ECLEVEL_H)
         * @return True if QR code was created successfully, false otherwise
         */
        static bool dataToQRFile(
            const std::vector<uint8_t> &data,
            const std::string &filename);

        /**
         * Reads a QR code from a PNG file and decodes it to binary data.
         *
         * @param filename Path to the QR code PNG file
         * @return The decoded binary data, or empty vector on error
         */
        static std::vector<uint8_t> qrFileToData(const std::string &filename);

        /**
         * Process data in chunks, creating a QR code for each chunk.
         *
         * @param data Input binary data
         * @param chunkSize Size of each chunk in bytes
         * @param outputDir Directory where QR codes will be saved
         * @param baseFilename Base name for the QR code files
         * @return Number of QR codes generated
         */
        static int processDataChunks(
            const std::vector<uint8_t> &data,
            size_t chunkSize,
            const std::string &outputDir,
            const std::string &baseFilename);
    };
} // namespace PhysicalStorage

#endif // QR_CODE_STORAGE_HPP
