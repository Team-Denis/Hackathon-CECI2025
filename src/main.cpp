
#include "main.hpp"


int main(int argc, char* argv[]) {
    
    (void) argc;
    (void) argv;

    try {

        std::string inputFilePath = "res/test.txt";
        std::ifstream inputFile(inputFilePath, std::ios::binary);
        if (!inputFile) {
            throw std::runtime_error("[e] Impossible d'ouvrir le fichier texte : " + inputFilePath);
        }

        std::string textContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        inputFile.close();
        std::vector<byte> data = FileManagementHelper::StringToBytes(textContent);

        std::string encodedFilePath = "res/test.denis";
        DenisEncoder encoder(1);
        encoder.Encode(encodedFilePath, data, DenisExtensionType::TXT);

        std::cout << "[i] Fichier encodé avec succès : " << encodedFilePath << std::endl;

        DenisDecoder decoder(1);
        std::vector<byte> decodedData = decoder.Decode(encodedFilePath);

        std::string decodedText = FileManagementHelper::BytesToString(decodedData);

        std::cout << "[i] Contenu décodé : " << std::endl;
        std::cout << decodedText << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}