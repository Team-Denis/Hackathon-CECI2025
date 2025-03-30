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
        static bool fileToQR(
            const std::string &in,
            const std::string &out);

        /**
         * Reads a QR code from a PNG file and decodes it to binary data.
         *
         * @param filename Path to the QR code PNG file
         * @return The decoded binary data, or empty vector on error
         */
        static bool QRToFile(const std::string &in, const std::string &out);

    };
} // namespace PhysicalStorage

#endif // QR_CODE_STORAGE_HPP
