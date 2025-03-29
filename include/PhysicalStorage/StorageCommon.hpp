#ifndef STORAGE_COMMON_HPP
#define STORAGE_COMMON_HPP

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace PhysicalStorage {
    /**
     * Common constants for physical storage components
     */
    namespace Constants {
        // Default bit matrix dimensions
        constexpr size_t DEFAULT_MATRIX_WIDTH = 256; // Width in bits
        constexpr size_t DEFAULT_MATRIX_HEIGHT = 256; // Height in bits

        // Maximum QR code version and capacity
        constexpr int MAX_QR_VERSION = 40;

        // Visualization constants
        constexpr int DEFAULT_SCREEN_WIDTH = 800;
        constexpr int DEFAULT_SCREEN_HEIGHT = 800;

        // File format identifiers
        const std::string MATRIX_CHUNK_EXT = ".bin"; // Changed to match current implementation
        const std::string QR_CODE_EXT = ".pbm";
        const std::string METADATA_EXT = "_metadata.txt"; // Changed to match current implementation
    }

    /**
     * Common utility functions for physical storage
     */
    namespace Utils {
        /**
         * Creates a directory if it doesn't exist.
         *
         * @param path Directory path to create
         * @return True if directory exists or was created, false otherwise
         */
        inline bool ensureDirectoryExists(const std::string &path) {
            if (!std::filesystem::exists(path)) {
                return std::filesystem::create_directories(path);
            }
            return true;
        }

        /**
         * Generates a unique filename in a directory.
         *
         * @param directory Target directory
         * @param baseName Base name for the file
         * @param extension File extension (including dot)
         * @return A unique filename that doesn't exist in the directory
         */
        inline std::string uniqueFilename(
            const std::string &directory,
            const std::string &baseName,
            const std::string &extension) {
            std::string filename = baseName + extension;
            std::string fullPath = std::filesystem::path(directory) / filename;

            int counter = 1;
            while (std::filesystem::exists(fullPath)) {
                filename = baseName + "_" + std::to_string(counter) + extension;
                fullPath = std::filesystem::path(directory) / filename;
                counter++;
            }

            return fullPath;
        }
    }
} // namespace PhysicalStorage

#endif // STORAGE_COMMON_HPP
