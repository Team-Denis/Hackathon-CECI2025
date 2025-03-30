#ifndef DENIS_ENCODER_HPP
#define DENIS_ENCODER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "DenisExtension.hpp"
#include "FileManagementHelper.hpp"



class DenisEncoder {

    std::set<int> SUPPORTED_VERSIONS = {1, 2};

public:
    
    DenisEncoder(const int version);
    ~DenisEncoder();

    [[nodiscard]] int GetVersion();
    void Encode(std::string &fp, std::vector<byte> &data, DenisExtensionType type, int padding = 0);

private:
    
    [[nodiscard]] static std::vector<byte> GetVersion1Header(std::vector<byte> &data, DenisExtensionType type);
    [[nodiscard]] static std::vector<byte> GetVersion2Header(std::vector<byte> &data, DenisExtensionType type, int padding);
    [[nodiscard]] std::vector<byte> GetHeader(std::vector<byte> &data, DenisExtensionType type, int padding);

    int version_;
    
};


#endif