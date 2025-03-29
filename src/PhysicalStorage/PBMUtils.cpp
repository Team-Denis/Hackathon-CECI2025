#include "PhysicalStorage/PBMUtils.h"

namespace PhysicalStorage {
    // Parse a PBM file into a bit matrix
    std::vector<uint8_t> PBMUtils::parsePBMFile(const std::string &filename, int &width, int &height) {
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
}