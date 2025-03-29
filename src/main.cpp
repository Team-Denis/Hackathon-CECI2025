#include "../include/chunker.h"
#include <iostream>
#include <string>

int main(const int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <filename>" << std::endl;
        std::cout << "Example: " << argv[0] << " myfile.dat" << std::endl;
        std::cout << "This will split myfile.dat into chunks of size 256x256 bits" << std::endl;
        std::cout << "and automatically verify the integrity using MD5 checksums." << std::endl;
        return 1;
    }

    const std::string filename = argv[1];

    chunker::splitFileIntoBitMatrixChunks(filename);

    return 0;
}
