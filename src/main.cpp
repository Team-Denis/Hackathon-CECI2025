#include "main.hpp"

namespace fs = std::filesystem;

void printUsage(const char *progName) {
    std::cout << "Usage: " << progName << " <command> [options]" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  encode <input_file> <output_file>       - Encode a file using DENIS format" << std::endl;
    std::cout << "  decode <input_file> <output_file>       - Decode a DENIS file" << std::endl;
    std::cout << "  chunk <input_file> <output_dir>         - Split file into 256x256 bit matrices" << std::endl;
    std::cout << "  merge <chunks_dir> <output_file>        - Merge chunks back into a file" << std::endl;
    std::cout << "  qrcode <input_file> <output_file>       - Create a QR code from a file" << std::endl;
}

int main(int argc, char *argv[]) {
    try {
        // If no arguments, show usage
        if (argc < 2) {
            printUsage(argv[0]);
            return EXIT_SUCCESS;
        }

        std::string command = argv[1];

        // Handle DENIS encode
        if (command == "encode" && argc == 4) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];
            // Add QR code extension if not already present
            if (fs::path(outputFile).extension() != PhysicalStorage::Constants::QR_CODE_EXT) {
                outputFile += PhysicalStorage::Constants::QR_CODE_EXT;
            }

            std::cout << "[i] Reading file: " << inputFile << std::endl;
            std::ifstream inputFileStream(inputFile, std::ios::binary);
            if (!inputFileStream) {
                throw std::runtime_error("[e] Cannot open input file: " + inputFile);
            }

            std::string textContent((std::istreambuf_iterator(inputFileStream)),
                                    std::istreambuf_iterator<char>());
            inputFileStream.close();

            std::vector<byte> data = FileManagementHelper::StringToBytes(textContent);
            std::cout << "[i] File size: " << data.size() << " bytes" << std::endl;

            std::cout << "[i] Encoding file to: " << outputFile << std::endl;
            DenisEncoder encoder(1);
            encoder.Encode(outputFile, data, DenisExtensionType::TXT);
            std::cout << "[i] File encoded successfully: " << outputFile << std::endl;
        }
        // Handle DENIS decode
        else if (command == "decode" && argc == 4) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];

            std::cout << "[i] Decoding file: " << inputFile << std::endl;
            DenisDecoder decoder(1);
            std::vector<byte> decodedData = decoder.Decode(inputFile);

            std::cout << "[i] Writing decoded data to: " << outputFile << std::endl;
            std::ofstream outputFileStream(outputFile, std::ios::binary);
            if (!outputFileStream) {
                throw std::runtime_error("[e] Cannot create output file: " + outputFile);
            }

            std::string decodedText = FileManagementHelper::BytesToString(decodedData);
            outputFileStream.write(decodedText.c_str(), decodedText.size());
            outputFileStream.close();

            std::cout << "[i] File decoded successfully. Size: " << decodedData.size() << " bytes" << std::endl;
        }
        // Handle chunking
        else if (command == "chunk" && argc >= 3) {
            std::string inputFile = argv[2];
            std::string outputDir = argc > 3 ? argv[3] : "";

            std::cout << "[i] Splitting file into 256x256 bit matrices: " << inputFile << std::endl;
            PhysicalStorage::MatrixChunkingInfo info = PhysicalStorage::MatrixChunker::splitFileIntoMatrices(
                inputFile,
                PhysicalStorage::Constants::DEFAULT_MATRIX_WIDTH,
                PhysicalStorage::Constants::DEFAULT_MATRIX_HEIGHT,
                outputDir
            );

            std::cout << "[i] Chunking complete. Created " << info.totalChunks << " chunks in: "
                    << info.outputDir << std::endl;
        }
        // Handle merging
        else if (command == "merge" && argc == 4) {
            std::string chunksDir = argv[2];
            std::string outputFile = argv[3];

            std::cout << "[i] Merging chunks from: " << chunksDir << std::endl;
            bool success = PhysicalStorage::MatrixChunker::mergeAndVerify(chunksDir, outputFile);

            if (success) {
                std::cout << "[i] Chunks merged and verified successfully: " << outputFile << std::endl;
            } else {
                std::cout << "[e] Failed to merge chunks or verification failed" << std::endl;
                return EXIT_FAILURE;
            }
        }
        // Handle QR code creation
        else if (command == "qrcode" && argc == 4) {
            std::string inputFile = argv[2];
            std::string outputFile = argv[3];

            std::cout << "[i] Reading file: " << inputFile << std::endl;
            std::ifstream inputFileStream(inputFile, std::ios::binary);
            if (!inputFileStream) {
                throw std::runtime_error("[e] Cannot open input file: " + inputFile);
            }

            std::vector<uint8_t> fileData((std::istreambuf_iterator(inputFileStream)),
                                          std::istreambuf_iterator<char>());
            inputFileStream.close();

            std::cout << "[i] Creating QR code: " << outputFile << std::endl;
            bool success = PhysicalStorage::QRCodeStorage::dataToQRFile(fileData, outputFile);

            if (success) {
                std::cout << "[i] QR code created successfully: " << outputFile << std::endl;
            } else {
                std::cout << "[e] Failed to create QR code" << std::endl;
                return EXIT_FAILURE;
            }
        }
        // If command not recognized
        else {
            printUsage(argv[0]);
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
