#include "EGLManager.h"
#include "GPUCellularAutomaton.h"
#include "PhysicalStorage/QRCodeStorage.hpp"
#include "Encryption/EncryptionHelper.hpp"
#include "Encryption/Key.h"
#include "CellularAutomatonVisualizer.hpp"

#include "main.hpp"

int encode(std::string &src, std::string &dst, bool visualize = false) {
    std::ifstream file(src, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Generate a secure encryption key
    auto key = Key::generate();

    // Display key with better formatting
    std::cout << "┌─────────────────────────────────────────┐" << std::endl;
    std::cout << "│          !!! DECRYPTION KEY !!!         │" << std::endl;
    std::cout << "├─────────────────────────────────────────┤" << std::endl;
    std::cout << "│ " << std::left << std::setw(39) << key.toString() << " │" << std::endl;
    std::cout << "└─────────────────────────────────────────┘" << std::endl;
    std::cout << "⚠️  WARNING: SAVE THIS KEY NOW  ⚠️" << std::endl;
    std::cout << "Without this key, your file will be PERMANENTLY LOST" << std::endl;
    std::cout << "and IMPOSSIBLE to recover by ANY means." << std::endl;

    constexpr std::size_t chunk_size = BUFFER_SIZE / 8;
    std::vector<uint8_t> buffer(chunk_size);

    std::array<int, BUFFER_SIZE> current_grid{};
    std::array<int, BUFFER_SIZE> prev_grid{};
    GPUCellularAutomaton engine;

    std::vector<uint8_t> data;
    std::streamsize bytes_read = 0;

    // Initialize visualizer if requested
    CellularAutomatonVisualizer visualizer("Denis File Encoder Visualization");
    if (visualize) {
        visualizer.start();
    }

    // Calculate total number of chunks for progress tracking
    file.seekg(0, std::ios::end);
    std::streamsize fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    int totalChunks = (fileSize + chunk_size - 1) / chunk_size;
    int currentChunk = 0;

    while (file && (visualize ? visualizer.isRunning() : true)) {
        // Read chunk
        file.read(reinterpret_cast<char *>(buffer.data()), chunk_size);

        bytes_read = file.gcount();
        if (bytes_read == 0) break; // End of file

        EncryptionHelper::Encrypt(buffer, key.XORKey);

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

        // Upload arrays to the GPU
        engine.clearPrevGrid();
        engine.writeCurrGrid(current_grid);

        // Update visualizer with initial state if requested
        if (visualize) {
            engine.readPrevGrid(prev_grid);
            visualizer.updateGridState(current_grid, prev_grid, currentChunk, totalChunks, 0, key.iter);
        }

        // Run iterations
        for (int i = 0; i < key.iter && (visualize ? visualizer.isRunning() : true); i++) {
            engine.runForward();

            // Update visualizer on every iteration if requested
            if (visualize) {
                engine.readCurrGrid(current_grid);
                engine.readPrevGrid(prev_grid);
                visualizer.updateGridState(current_grid, prev_grid, currentChunk, totalChunks, i + 1, key.iter);
            }
        }

        // Check if visualizer was closed by user
        if (visualize && !visualizer.isRunning()) {
            break;
        }

        // Retrieve the current grid
        engine.readCurrGrid(current_grid);
        engine.readPrevGrid(prev_grid);

        for (size_t i = 0; i < prev_grid.size(); i += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && i + b < prev_grid.size(); ++b) {
                byte |= prev_grid[i + b] << 7 - b;
            }
            data.push_back(byte);
        }

        for (size_t i = 0; i < current_grid.size(); i += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && i + b < current_grid.size(); ++b) {
                byte |= current_grid[i + b] << 7 - b;
            }
            data.push_back(byte);
        }

        currentChunk++;
    }

    // Stop visualizer
    if (visualize) {
        visualizer.stop();
    }

    DenisEncoder enc(2);
    enc.Encode(dst, data, DenisExtensionType::ANY, chunk_size - bytes_read);

    file.close();
    std::cout << "Encoding complete! File saved to: " << dst << std::endl;
    return 0;
}

int decode(std::string &src, std::string &dst, const Key &key, bool visualize = false) {
    std::ofstream file(dst, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file.\n";
        return 1;
    }

    // Display decoding information
    std::cout << "┌─────────────────────────────────────────┐" << std::endl;
    std::cout << "│         DECODING WITH KEY               │" << std::endl;
    std::cout << "├─────────────────────────────────────────┤" << std::endl;
    std::cout << "│ " << std::left << std::setw(39) << key.toString() << " │" << std::endl;
    std::cout << "└─────────────────────────────────────────┘" << std::endl;

    DenisDecoder dec(2);
    std::array<int, BUFFER_SIZE> grid{};
    GPUCellularAutomaton engine;

    // Initialize visualizer if requested
    CellularAutomatonVisualizer visualizer("Denis File Decoder Visualization");
    if (visualize) {
        visualizer.start();
    }

    // Decode the input file
    std::cout << "Reading encoded file..." << std::endl;
    auto [header, encoded_bytes] = dec.Decode(src);
    std::vector<uint8_t> decoded_bytes;

    // Calculate total number of chunks for progress tracking
    int i = 0;
    int totalChunks = encoded_bytes.size() / (BUFFER_SIZE / 4); // Approximation
    int currentChunk = 0;

    while (i < encoded_bytes.size() && (visualize ? visualizer.isRunning() : true)) {
        std::size_t bitIndex = 0;
        for (std::streamsize j = 0; j < BUFFER_SIZE / 8; j++) {
            // Convert each byte to bits
            std::bitset<8> bits(encoded_bytes[i++]);
            for (int b = 7; b >= 0; --b) {
                grid[bitIndex++] = bits[b];
            }
        }
        engine.writePrevGrid(grid);

        bitIndex = 0;
        for (std::streamsize j = 0; j < BUFFER_SIZE / 8; j++) {
            // Convert each byte to bits
            std::bitset<8> bits(encoded_bytes[i++]);
            for (int b = 7; b >= 0; --b) {
                grid[bitIndex++] = bits[b];
            }
        }
        engine.writeCurrGrid(grid);

        // Update visualizer with initial state if requested
        if (visualize) {
            std::array<int, BUFFER_SIZE> prev_grid{};
            engine.readPrevGrid(prev_grid);
            engine.readCurrGrid(grid);
            visualizer.updateGridState(grid, prev_grid, currentChunk, totalChunks, 0, key.iter);
        }

        for (int j = 0; j < key.iter && (visualize ? visualizer.isRunning() : true); j++) {
            engine.runBackward();

            // Update visualizer on every iteration if requested
            if (visualize) {
                std::array<int, BUFFER_SIZE> prev_grid{};
                engine.readPrevGrid(prev_grid);
                engine.readCurrGrid(grid);
                visualizer.updateGridState(grid, prev_grid, currentChunk, totalChunks, j + 1, key.iter);
            }
        }

        // Check if visualizer was closed by user
        if (visualize && !visualizer.isRunning()) {
            break;
        }

        engine.readCurrGrid(grid);

        bool is_final_chunk = encoded_bytes.size() - i <= BUFFER_SIZE / 8;
        int padding = is_final_chunk ? header.padding * 8 : 0;

        for (size_t j = 0; j < grid.size() - padding; j += 8) {
            uint8_t byte = 0;
            for (int b = 0; b < 8 && j + b < grid.size(); ++b) {
                byte |= grid[j + b] << 7 - b;
            }
            decoded_bytes.push_back(byte);
        }

        EncryptionHelper::Decrypt(decoded_bytes, key.XORKey);
        file.write(reinterpret_cast<char *>(decoded_bytes.data()), decoded_bytes.size());
        decoded_bytes.clear();

        currentChunk++;
    }

    // Stop visualizer
    if (visualize) {
        visualizer.stop();
    }

    file.close();
    std::cout << "\nDecoding complete! File saved to: " << dst << std::endl;
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

    program.add_argument("--visualize").flag()
            .help("Visualize the cellular automaton process");

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
        bool visualize = program.get<bool>("--visualize");

        auto input = program.get<std::string>("input");
        auto output = program.get<std::string>("output");

        if (is_encode) {
            int ret = encode(input, output, visualize);

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

        if (qr) {
            std::cout << "Reading QR code..." << std::endl;
            PhysicalStorage::QRCodeStorage::QRToFile(input, temp_dest);
        }

        auto key = program.get<std::string>("--key");
        int ret = decode(qr ? temp_dest : input, output, Key(key), visualize);
        EGLManager::cleanup();
        return ret;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << program;
        EGLManager::cleanup();
        return 1;
    }
}
