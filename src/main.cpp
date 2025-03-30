#include "main.hpp"
#include "EGLManager.h"
#include "GPUCellularAutomaton.h"

namespace fs = std::filesystem;

void printUsage(const char *progName) {
    std::cout << "Usage: " << progName << " <command> [options]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  encode <input_file> <output_file> <steps>" << std::endl;
    std::cout << "    - Encrypts the input file using CA, encodes with DENIS format," << std::endl;
    std::cout << "      and generates a QR code output." << std::endl;
    std::cout << "  decode <input_file> <output_file> <steps>" << std::endl;
    std::cout << "    - Decodes a DENIS file and decrypts using CA in reverse." << std::endl;
    std::cout << "Parameters:" << std::endl;
    std::cout << "  <input_file>  - Path to the input file" << std::endl;
    std::cout << "  <output_file> - Path for the output file" << std::endl;
    std::cout << "  <steps>       - Number of CA steps (encryption key)" << std::endl;
}

int main(int argc, char *argv[]) {
    try {
        // Check for minimum number of arguments
        if (argc < 2) {
            printUsage(argv[0]);
            return EXIT_SUCCESS;
        }

        std::string command = argv[1];

        // ENCODE COMMAND - Process, encrypt, and encode
        if (command == "encode" && argc == 5) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];
            int steps = std::stoi(argv[4]); // Parse steps parameter

            if (steps < 1) {
                std::cerr << "[e] Steps must be a positive integer" << std::endl;
                return EXIT_FAILURE;
            }

            std::string qrOutputFile = outputFile + PhysicalStorage::Constants::QR_CODE_EXT;

            std::cout << "[i] Analyzing file: " << inputFile << std::endl;
            std::cout << "[i] Using " << steps << " CA steps as encryption key" << std::endl;

            // Analyze the file to determine chunking requirements
            auto fileInfo = PhysicalStorage::MatrixChunker::analyzeFile(inputFile);

            // Initialize EGL for GPU processing
            std::cout << "[i] Initializing GPU environment..." << std::endl;
            EGLManager::init();

            // Process each chunk through the cellular automaton
            std::vector<std::array<GLint, BUFFER_SIZE> > processedChunks;
            // Reserve twice as much space since we're storing both prev and current grids
            processedChunks.reserve(fileInfo.totalChunks * 2);

            std::cout << "[i] Processing chunks through cellular automaton..." << std::endl;

            for (size_t i = 0; i < fileInfo.totalChunks; i++) {
                std::cout << "[i] Processing chunk " << i + 1 << " of " << fileInfo.totalChunks << std::endl;

                // Read chunk from file as a 256x256 bit array
                auto chunkData = PhysicalStorage::MatrixChunker::readChunk<SIDE, SIDE>(
                    inputFile, i, SIDE, SIDE);

                // Convert to GLint array for CA
                std::array<GLint, BUFFER_SIZE> glChunk;
                for (size_t j = 0; j < BUFFER_SIZE; j++) {
                    glChunk[j] = chunkData[j];
                }

                // Process chunk through CA using the specified number of steps
                GPUCellularAutomaton ca(glChunk);
                for (int step = 0; step < steps; step++) {
                    ca.run_forward();
                }

                // Read both prev and current grids
                std::array<GLint, BUFFER_SIZE> processedChunkPrev;
                ca.read_prev_grid(processedChunkPrev);

                std::array<GLint, BUFFER_SIZE> processedChunkCurrent;
                ca.read_current_grid(processedChunkCurrent);

                // Store both grids for reversibility
                processedChunks.push_back(processedChunkPrev);
                processedChunks.push_back(processedChunkCurrent);
            }

            // Clean up GPU resources
            EGLManager::cleanup();

            // Merge the processed chunks
            std::cout << "[i] Merging processed chunks..." << std::endl;
            std::vector<uint8_t> mergedData = PhysicalStorage::MatrixChunker::mergeChunks<SIDE, SIDE>(
                processedChunks, fileInfo.fileSizeBits * 2); // Double the bits since we have twice as many grids

            // Encode using DENIS format
            std::cout << "[i] Encoding with DENIS format to: " << outputFile << std::endl;
            DenisEncoder encoder(1);
            encoder.Encode(outputFile, mergedData, DenisExtensionType::TXT);

            // Generate QR code
            std::cout << "[i] Generating QR code: " << qrOutputFile << std::endl;
            bool qrSuccess = PhysicalStorage::QRCodeStorage::dataToQRFile(mergedData, qrOutputFile);

            if (qrSuccess) {
                std::cout << "[i] Encryption and encoding complete." << std::endl;
                std::cout << "[i] Encoded file: " << outputFile << std::endl;
                std::cout << "[i] QR code: " << qrOutputFile << std::endl;
                std::cout << "[i] To decode this file, you will need the key: " << steps << std::endl;
            } else {
                std::cerr << "[e] Failed to create QR code." << std::endl;
                return EXIT_FAILURE;
            }
        } // Invalid command or incorrect number of arguments
        else {
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }
    } catch (const std::exception &e) {
        std::cerr << "[e] Exception: " << e.what() << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << "[e] Unknown error occurred." << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
