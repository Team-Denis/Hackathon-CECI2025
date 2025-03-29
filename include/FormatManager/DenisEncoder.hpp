#ifndef DENIS_ENCODER_HPP
#define DENIS_ENCODER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "DenisExtension.hpp"
#include "FileManagementHelper.hpp"



class DenisEncoder {

    std::set<int> SUPPORTED_VERSIONS = {1};

public:
    
    DenisEncoder(const int version);
    ~DenisEncoder();

    [[nodiscard]] int GetVersion();
    void Encode(std::string &fp, std::vector<byte> &data, DenisExtensionType type);

private:
    
    [[nodiscard]] static std::vector<byte> GetVersion1Header(std::vector<byte> &data, DenisExtensionType type);
    [[nodiscard]] std::vector<byte> GetHeader(std::vector<byte> &data, DenisExtensionType type);

    int version_;
    
};


#endif