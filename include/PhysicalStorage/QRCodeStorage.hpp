/*
* This data storage is inspired by the GitHub Arctic Vault : 
* https://archiveprogram.github.com/arctic-vault/
*/

#ifndef QR_CODE_STORAGE_HPP
#define QR_CODE_STORAGE_HPP

#include "PhysicalStorage/StorageCommon.hpp"
#include <qrencode.h>
#include <zbar.h>
#include <string>
#include <vector>
#include <filesystem>

namespace PhysicalStorage {
    /**
     * Handles conversion between binary data and QR codes for archival storage
     */
    class QRCodeStorage {
    public:
        /**
         * Creates a QR code from binary data and writes it to a file in PBM format.
         *
         * @param data Binary data to encode
         * @param filename Output filename for the QR code
         * @param errorCorrectionLevel QR code error correction level (default: QR_ECLEVEL_H)
         * @return True if QR code was created successfully, false otherwise
         */
        static bool dataToQRFile(
            const std::vector<uint8_t> &data,
            const std::string &filename,
            QRecLevel errorCorrectionLevel = QR_ECLEVEL_H);

        /**
         * Reads a QR code from a PBM file and decodes it to binary data.
         * Note: This requires a QR code reader, which is not implemented in this example.
         *
         * @param filename Path to the QR code PBM file
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

    private:
        /**
         * Writes a QRcode struct to a PBM file.
         *
         * @param qr QRcode struct to write
         * @param filename Output filename
         * @return True if file was written successfully, false otherwise
         */
        static bool writeQRToFile(const QRcode &qr, const std::string &filename);
    };
} // namespace PhysicalStorage

#endif // QR_CODE_STORAGE_HPP
