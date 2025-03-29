#ifndef QR_CODE_VISUALIZER_HPP
#define QR_CODE_VISUALIZER_HPP

#include "PhysicalStorage/StorageCommon.hpp"
#include <raylib.h>

namespace PhysicalStorage {
    /**
     * Provides visualization capabilities for QR codes and matrix data.
     */
    class QRCodeVisualizer {
    public:
        /**
         * Renders a QR code from a PBM file using raylib.
         *
         * @param filename Path to the PBM file containing the QR code
         * @param windowTitle Window title (default: "QR Code Visualization")
         * @param width Window width (default: 800)
         * @param height Window height (default: 800)
         */
        static void displayQRCode(
            const std::string &filename,
            const std::string &windowTitle = "QR Code Visualization",
            int width = Constants::DEFAULT_SCREEN_WIDTH,
            int height = Constants::DEFAULT_SCREEN_HEIGHT);

        /**
         * Renders a binary matrix directly from memory using raylib.
         *
         * @param data Binary data to visualize as a matrix
         * @param width Matrix width in bits
         * @param height Matrix height in bits
         * @param windowTitle Window title (default: "Matrix Visualization")
         * @param screenWidth Window width (default: 800)
         * @param screenHeight Window height (default: 800)
         */
        static void displayBitMatrix(
            const std::vector<uint8_t> &data,
            size_t width,
            size_t height,
            const std::string &windowTitle = "Matrix Visualization",
            int screenWidth = Constants::DEFAULT_SCREEN_WIDTH,
            int screenHeight = Constants::DEFAULT_SCREEN_HEIGHT);

        /**
         * Saves a visualization of a bit matrix to an image file.
         *
         * @param data Binary data to visualize as a matrix
         * @param width Matrix width in bits
         * @param height Matrix height in bits
         * @param outputFilename Path to save the visualization image
         * @param imageWidth Image width in pixels (default: 800)
         * @param imageHeight Image height in pixels (default: 800)
         * @return True if the image was saved successfully, false otherwise
         */
        static bool saveBitMatrixImage(
            const std::vector<uint8_t> &data,
            size_t width,
            size_t height,
            const std::string &outputFilename,
            int imageWidth = Constants::DEFAULT_SCREEN_WIDTH,
            int imageHeight = Constants::DEFAULT_SCREEN_HEIGHT);
    };
} // namespace PhysicalStorage

#endif // QR_CODE_VISUALIZER_HPP
