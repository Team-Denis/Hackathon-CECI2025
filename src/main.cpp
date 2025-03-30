#include "main.hpp"
#include "EGLManager.h"
#include "GPUCellularAutomaton.h"
#include "PhysicalStorage/QRCodeStorage.hpp"
#include "Encryption/EncryptionHelper.hpp"
#include "Encryption/Key.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <argparse/argparse.hpp>

int encode(std::string& src, std::string& dst) {
    std::ifstream file(src, std::ios::binary);
    if (!file) {
      std::cerr << "Failed to open file.\n";
      return 1;
    }

    constexpr std::size_t chunk_size = BUFFER_SIZE / 8;
    std::vector<uint8_t> buffer(chunk_size);

    std::array<int, BUFFER_SIZE> current_grid{0};
    std::array<int, BUFFER_SIZE> prev_grid{0};
    GPUCellularAutomaton engine;

    std::vector<uint8_t> data;
    std::streamsize bytes_read = 0;

    auto key = Key::gen();
    std::cout << "DECRYPTION KEY: " << key.to_string() << std::endl;

    while (file) {
        // Read chunk
        file.read(reinterpret_cast<char*>(buffer.data()), chunk_size);

        EncryptionHelper::Encrypt(buffer, key.xor_key);

        bytes_read = file.gcount();
        std::size_t bitIndex = 0;
        for (std::streamsize i = 0; i < chunk_size; i++) {
            // Convert each byte to bits
            std::bitset<8> bits(buffer[i]);
            for (int b = 7; b >= 0; --b) {
                if (i < bytes_read)
                    current_grid[bitIndex++] = bits[b];
                else
                    current_grid[bitIndex++] = 0;
            }
        }

        // Upload arrays to the gpu
        engine.clear_prev_grid();
        engine.upload_current_grid(current_grid);

        // Run interations
        for (int i = 0; i < key.iter; i++) {
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
    enc.Encode(dst, data, DenisExtensionType::ANY, chunk_size - bytes_read);

    file.close();
    return 0;
}

int decode(std::string& src, std::string& dst, const Key& key) {
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

        for (int j = 0; j < key.iter; j++) {
            engine.run_backward();
        }

        engine.read_current_grid(grid);

        bool is_final_chunk = encoded_bytes.size() - i <= BUFFER_SIZE / 8;
        int padding = is_final_chunk ? (header.padding * 8) : 0;

        for (size_t j = 0; j < grid.size() - padding; j += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && (j + b) < grid.size(); ++b) {
                byte |= (grid[j + b] << (7 - b));
            }
            decoded_bytes.push_back(byte);
        }

        EncryptionHelper::Decrypt(decoded_bytes, key.xor_key);
        file.write(reinterpret_cast<char*>(decoded_bytes.data()), decoded_bytes.size());
    }

    return 0;
}

int main(int argc, char **argv) {
    EGLManager::init();

    argparse::ArgumentParser program("denis");

    auto &group = program.add_mutually_exclusive_group(true);
    group.add_argument("-e", "--encode").flag();
    group.add_argument("-d", "--decode").flag();

    program.add_argument("--qr").flag()
        .help("Generate or read from a QR code");

    program.add_argument("input")
        .required()
        .help("Input file path");

    program.add_argument("output")
        .required()
        .help("Output file path");

    program.add_argument("--key")
        .help("Decryption key (required for decoding)");

    try {
        program.parse_args(argc, argv);

        bool is_encode = program.get<bool>("-e");
        bool qr = program.get<bool>("--qr");

        auto input = program.get<std::string>("input");
        auto output = program.get<std::string>("output");

        if (is_encode) {
            int ret = encode(input, output);

            if (qr)
                PhysicalStorage::QRCodeStorage::fileToQR(output, output + ".png");

            EGLManager::cleanup();
            return ret;
        }

        if (!program.present("--key")) {
          throw std::runtime_error(
              "Missing required argument: --key (needed for decoding)");
        }

        auto temp_dest = output + ".data";

        if (qr)
            PhysicalStorage::QRCodeStorage::QRToFile(input, temp_dest);

        auto key = program.get<std::string>("--key");
        EGLManager::cleanup();
        return decode(qr ? temp_dest : input, output, Key(key));
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << program;
        EGLManager::cleanup();
        return 1;
    }
}