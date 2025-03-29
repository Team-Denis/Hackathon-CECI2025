#ifndef PBMUTILS_H
#define PBMUTILS_H

#include "StorageCommon.hpp"


namespace PhysicalStorage {
    class PBMUtils {
    public:
        /**
         * Parses a PBM file into a bit matrix.
         *
         * @param filename Path to the PBM file
         * @param width Output parameter to store the matrix width
         * @param height Output parameter to store the matrix height
         * @return Vector containing the parsed bit data
         */
        static std::vector<uint8_t> parsePBMFile(
            const std::string &filename,
            int &width,
            int &height);    
    };
}



#endif // PBMUTILS_H