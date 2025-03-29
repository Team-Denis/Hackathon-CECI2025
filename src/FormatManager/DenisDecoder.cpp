#include "../../include/FormatManager/DenisDecoder.hpp"


DenisDecoder::DenisDecoder(const int version) : version_(version) {
    
    // constructor of DenisDecoder, prolly need to wrtie stuff but yeah anyway
    
    if (SUPPORTED_VERSIONS.find(version) == SUPPORTED_VERSIONS.end()) {
        throw std::invalid_argument("[e] Unsupported version :" + std::to_string(version));
    }

}

DenisDecoder::~DenisDecoder() {
    // destructor of DenisDecoder, I'll never write stuff here trust me (I'm killing myself if 
    // I do)
}



int DenisDecoder::GetVersion() {
    return version_;
}

DenisHeader DenisDecoder::ReadVersion1Header(std::vector<byte> &buffer) {
    
    if (buffer.size() != HEADER_LENGTH) {
        throw std::runtime_error("[e] Buffer length is incorrect: " + std::to_string(buffer.size()) +
                                 " instead of " + std::to_string(HEADER_LENGTH));
    }

    std::string magic = FileManagementHelper::BytesToString({buffer.begin(), buffer.begin() + 5});
    if (magic != DENIS_MAGIC_STRING) {
        throw std::runtime_error("[e] Invalid magic string: " + magic + ". Expected: " + DENIS_MAGIC_STRING);
    }

    int version = FileManagementHelper::BytesToInt({buffer.begin() + 5, buffer.begin() + 6}, 1);
    std::string formatStr = FileManagementHelper::BytesToString({buffer.begin() + 6, buffer.begin() + 9});
    DenisExtensionType type = EXTENSION_MAP_REVERSE.find(formatStr) != EXTENSION_MAP_REVERSE.end() ? EXTENSION_MAP_REVERSE.at(formatStr) : DenisExtensionType::NONE;
    if (type == DenisExtensionType::NONE) {
        throw std::runtime_error("[e] Invalid format string: " + formatStr);
    }

    std::vector<byte> extra(buffer.begin() + 9, buffer.begin() + 16);
    int dataSize = FileManagementHelper::BytesToInt({buffer.begin() + 16, buffer.begin() + 24}, 8);

    return {magic, version, type, dataSize, extra};
}

DenisHeader DenisDecoder::ReadHeader(std::vector<byte> &buffer) {
    
    switch (GetVersion()) {
        case 1:
            return ReadVersion1Header(buffer);
        default:
            throw std::invalid_argument("[e] Unsupported version: " + std::to_string(GetVersion()));
    }

}

std::vector<byte> DenisDecoder::Decode(std::string &fp) {

    std::vector<byte> buffer = FileManagementHelper::ReadBuffer(fp, std::filesystem::file_size(fp));
    if (buffer.empty()) {
        throw std::runtime_error("[e] Buffer is empty.");
    }

    // Read and validate the header
    std::vector<byte> headerBuffer(buffer.begin(), buffer.begin() + HEADER_LENGTH);
    DenisHeader header = ReadHeader(headerBuffer);

    // Extract the content
    std::vector<byte> content(buffer.begin() + HEADER_LENGTH, buffer.begin() + HEADER_LENGTH + header.data_size);

    // Validate the terminator
    std::vector<byte> terminator(buffer.begin() + HEADER_LENGTH + header.data_size, buffer.end());
    if (terminator.size() != 1 || terminator[0] != 0xFF) {
        throw std::runtime_error("[e] Invalid terminator.");
    }

    return content;
}