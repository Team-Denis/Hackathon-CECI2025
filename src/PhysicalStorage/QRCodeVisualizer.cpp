#include "PhysicalStorage/QRCodeVisualizer.hpp"

namespace PhysicalStorage {
    // Implementation of QRCodeVisualizer class methods
    void QRCodeVisualizer::displayQRCode(const std::string &filename, const std::string &windowTitle, int width,
                                         int height) {
        InitWindow(width, height, windowTitle.c_str());
        SetTargetFPS(60);

        // Read the PBM file
        int qrWidth = 0;
        int qrHeight = 0;
        std::vector<uint8_t> qrData = parsePBMFile(filename, qrWidth, qrHeight);

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

    // Parse a PBM file into a bit matrix
    std::vector<uint8_t> QRCodeVisualizer::parsePBMFile(const std::string &filename, int &width, int &height) {
        std::ifstream file(filename);
        if (!file) {
            std::cerr << "Cannot open PBM file: " << filename << std::endl;
            return {};
        }

        // Read PBM header
        std::string line;
        std::getline(file, line); // P1 format identifier
        if (line != "P1") {
            std::cerr << "Not a valid PBM (P1) file" << std::endl;
            return {};
        }

        // Skip comments
        do {
            std::getline(file, line);
        } while (line[0] == '#');

        // Parse width and height
        std::istringstream iss(line);
        iss >> width >> height;

        if (width <= 0 || height <= 0) {
            std::cerr << "Invalid PBM dimensions: " << width << "x" << height << std::endl;
            return {};
        }

        // Read the data
        std::vector<uint8_t> data;
        data.reserve(width * height);

        int value;
        while (file >> value) {
            data.push_back(static_cast<uint8_t>(value));
        }

        if (data.size() != static_cast<size_t>(width * height)) {
            std::cerr << "PBM data size mismatch. Expected " << (width * height)
                    << " values, got " << data.size() << std::endl;
        }

        return data;
    }
} // namespace PhysicalStorage
