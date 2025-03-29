#include "main.hpp"

int main(int argc, char *argv[]) {
    std::string inputFilePath = "res/test.txt";
    std::string encodedFilePath = "res/test.denis";

    // Process command line arguments if provided
    if (argc > 1) {
        inputFilePath = argv[1];
    }
    if (argc > 2) {
        encodedFilePath = argv[2];
    }

    try {
        // Read the input file
        std::cout << "[i] Reading file: " << inputFilePath << std::endl;
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile) {
            throw std::runtime_error("[e] Impossible d'ouvrir le fichier : " + inputFilePath);
        }

        // Read file content into string
        std::string textContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        inputFile.close();

        // Convert string to bytes
        std::vector<byte> data = FileManagementHelper::StringToBytes(textContent);
        std::cout << "[i] File size: " << data.size() << " bytes" << std::endl;

        // Encode file
        std::cout << "[i] Encoding file to: " << encodedFilePath << std::endl;
        DenisEncoder encoder(1);
        encoder.Encode(encodedFilePath, data, DenisExtensionType::TXT);
        std::cout << "[i] File successfully encoded: " << encodedFilePath << std::endl;

        // Decode file to verify
        std::cout << "[i] Decoding file for verification..." << std::endl;
        DenisDecoder decoder(1);
        std::vector<byte> decodedData = decoder.Decode(encodedFilePath);

        // Convert bytes back to string
        std::string decodedText = FileManagementHelper::BytesToString(decodedData);

        // Check if decoded text matches original
        bool success = (decodedText == textContent);

        std::cout << "[i] Verification " << (success ? "successful" : "failed") << std::endl;

        if (success) {
            std::cout << "[i] Original and decoded data match. Size: " << decodedData.size() << " bytes" << std::endl;
        } else {
            std::cout << "[e] Mismatch between original and decoded data!" << std::endl;
            std::cout << "[i] Original size: " << textContent.size() << " bytes" << std::endl;
            std::cout << "[i] Decoded size: " << decodedText.size() << " bytes" << std::endl;
        }

        // Print a short preview of the decoded text (first 100 characters)
        std::cout << "[i] Decoded text preview: " << std::endl;
        std::cout << decodedText.substr(0, std::min(100UL, decodedText.size())) <<
                (decodedText.size() > 100 ? "..." : "") << std::endl;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
