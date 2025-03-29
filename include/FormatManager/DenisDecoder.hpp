#ifndef DENIS_DECODER_HPP
#define DENIS_DECODER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

#include "DenisExtension.hpp"
#include "FileManagementHelper.hpp"


struct DenisHeader {
    std::string magic_string;
    int version;
    DenisExtensionType type;
    int data_size;
    std::vector<byte> extra;
};


class DenisDecoder {

    std::set<int> SUPPORTED_VERSIONS = {1};

public:
    
    DenisDecoder(const int version);
    ~DenisDecoder();

    [[nodiscard]] int GetVersion();
    std::vector<byte> Decode(std::string &fp);

private:
    
    [[nodiscard]] static DenisHeader ReadVersion1Header(std::vector<byte> &buffer);
    [[nodiscard]] DenisHeader ReadHeader(std::vector<byte> &buffer);

    int version_;
    
};


#endif