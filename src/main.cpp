#include "main.hpp"
#include "EGLManager.h"
#include "GPUCellularAutomaton.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>

template <typename T, std::size_t N>
void printArray(const std::array<T, N>& arr) {
    std::cout << "[ ";
    for (const auto& elem : arr) {
        std::cout << elem << " ";
    }
    std::cout << "]\n";
}

template <typename T>
void printVector(const std::vector<T>& vec) {
    std::cout << "[ ";
    for (const auto& elem : vec) {
        std::cout << elem << " ";
    }
    std::cout << "]\n";
}


int encode(std::string& src, std::string& dst) {
    std::ifstream file(src, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    const std::size_t chunkSize = BUFFER_SIZE / 8;
    std::vector<uint8_t> buffer(chunkSize);

    std::array<int, BUFFER_SIZE> current_grid{0};
    std::array<int, BUFFER_SIZE> prev_grid{0};
    GPUCellularAutomaton engine;

    std::vector<uint8_t> data;
    std::streamsize bytes_read = 0;

    while (file) {
        // Read chunk
        file.read(reinterpret_cast<char*>(buffer.data()), chunkSize);
        bytes_read = file.gcount();
        std::size_t bitIndex = 0;
        for (std::streamsize i = 0; i < bytes_read; i++) {
            // Convert each byte to bits
            std::bitset<8> bits(buffer[i]);
            for (int b = 7; b >= 0; --b) {
                current_grid[bitIndex++] = bits[b];
            }
        }

        // Upload arrays to the gpu
        engine.clear_prev_grid();
        engine.upload_current_grid(current_grid);

        // Run interations
        for (int i = 0; i < 100; i++) {
            engine.run_forward();
        }

        // Retrieve the current grid
        engine.read_current_grid(current_grid);
        engine.read_prev_grid(prev_grid);

        for (size_t i = 0; i < prev_grid.size(); i += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && (i + b) < prev_grid.size(); ++b) {
                byte |= (prev_grid[i + b] << (7 - b));
            }
            data.push_back(byte);
        }

        for (size_t i = 0; i < current_grid.size(); i += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && (i + b) < current_grid.size(); ++b) {
                byte |= (current_grid[i + b] << (7 - b));
            }
            data.push_back(byte);
        }
    }

    DenisEncoder enc(2);
    enc.Encode(dst, data, DenisExtensionType::TXT, chunkSize - bytes_read);

    file.close();
    return 0;
}

int decode(std::string& src, std::string& dst) {
    std::ofstream file(dst, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    DenisDecoder dec(2);

    std::array<int, BUFFER_SIZE> grid{0};
    GPUCellularAutomaton engine;

    auto [header, encoded_bytes] = dec.Decode(src);
    std::vector<uint8_t> decoded_bytes;

    int i = 0;

    while (i < encoded_bytes.size()) {
        std::size_t bitIndex = 0;
        for (std::streamsize j = 0; j < BUFFER_SIZE / 8; j++) {
            // Convert each byte to bits
            std::bitset<8> bits(encoded_bytes[i++]);
            for (int b = 7; b >= 0; --b) {
                grid[bitIndex++] = bits[b];
            }
        }
        engine.upload_prev_grid(grid);

        bitIndex = 0;
        for (std::streamsize j = 0; j < BUFFER_SIZE / 8; j++) {
            // Convert each byte to bits
            std::bitset<8> bits(encoded_bytes[i++]);
            for (int b = 7; b >= 0; --b) {
                grid[bitIndex++] = bits[b];
            }
        }
        engine.upload_current_grid(grid);

        for (int i = 0; i < 100; i++) {
            engine.run_backward();
        }

        engine.read_current_grid(grid);

        for (size_t i = 0; i < grid.size() - (header.padding * 8); i += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && (i + b) < grid.size(); ++b) {
                byte |= (grid[i + b] << (7 - b));
            }
            file.put(byte);
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    EGLManager::init();

    auto src = std::string("../README.md");
    auto mid = std::string("out.denis");
    auto dest = std::string("out.txt");
    encode(src, mid);
    decode(mid, dest);

    EGLManager::cleanup();
}