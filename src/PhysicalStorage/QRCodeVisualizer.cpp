#include "PhysicalStorage/QRCodeVisualizer.hpp"
#include "PhysicalStorage/PBMUtils.h"

namespace PhysicalStorage {
    // Implementation of QRCodeVisualizer class methods
    void QRCodeVisualizer::displayQRCode(const std::string &filename, const std::string &windowTitle, int width,
                                         int height) {
        InitWindow(width, height, windowTitle.c_str());
        SetTargetFPS(60);

        // Read the PBM file
        int qrWidth = 0;
        int qrHeight = 0;
        std::vector<uint8_t> qrData = PBMUtils::parsePBMFile(filename, qrWidth, qrHeight);

        if (qrData.empty() || qrWidth == 0 || qrHeight == 0) {
            CloseWindow();
            return;
        }

        // Render the QR code until window is closed
        while (!WindowShouldClose()) {
            BeginDrawing();
            ClearBackground(RAYWHITE);

            // Calculate cell size based on window dimensions and QR code size
            float cellWidth = static_cast<float>(width) / qrWidth;
            float cellHeight = static_cast<float>(height) / qrHeight;

            // Draw each QR code cell
            for (int y = 0; y < qrHeight; y++) {
                for (int x = 0; x < qrWidth; x++) {
                    int index = y * qrWidth + x;
                    if (index < static_cast<int>(qrData.size()) && qrData[index] == 1) {
                        DrawRectangle(
                            static_cast<int>(x * cellWidth),
                            static_cast<int>(y * cellHeight),
                            static_cast<int>(cellWidth + 1),
                            static_cast<int>(cellHeight + 1),
                            BLACK
                        );
                    }
                }
            }

            EndDrawing();
        }

        CloseWindow();
    }

    // Implementation of other methods would go here


} // namespace PhysicalStorage
