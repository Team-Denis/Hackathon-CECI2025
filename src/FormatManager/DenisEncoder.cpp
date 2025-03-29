
#include "../../include/FormatManager/DenisEncoder.hpp"


DenisEncoder::DenisEncoder(const int version) : version_(version) {
    
    // constructor of DenisEncoder, prolly need to wrtie stuff but yeah anyway
    
    if (SUPPORTED_VERSIONS.find(version) == SUPPORTED_VERSIONS.end()) {
        throw std::invalid_argument("[e] Unsupported version :" + std::to_string(version));
    }

}

DenisEncoder::~DenisEncoder() {
    // destructor of DenisEncoder, I'll never write stuff here trust me (I'm killing myself if 
    // I do)
}



int DenisEncoder::GetVersion() {
    return version_;
}

void DenisEncoder::Encode(std::string &fp, std::vector<byte> &data, DenisExtensionType type) {
    
    // encode the data and write it to the file
    if (type == DenisExtensionType::NONE) {
        throw std::invalid_argument("[e] Format cannot be NONE.");
    }

    std::vector<byte> bufferToWrite;
    std::vector<byte> header = GetHeader(data, type);

    bufferToWrite.insert(bufferToWrite.end(), header.begin(), header.end());    // header
    bufferToWrite.insert(bufferToWrite.end(), data.begin(), data.end());        // data to write
    bufferToWrite.insert(bufferToWrite.end(), 8, 0xFF);                         // terminator

    int bytesWritten = FileManagementHelper::WriteBuffer(fp, bufferToWrite);
    
    if (bytesWritten != bufferToWrite.size()) {
        throw std::runtime_error("[e] Bytes written do not match buffer size.");
    }

}


std::vector<byte> DenisEncoder::GetVersion1Header(std::vector<byte> &data, DenisExtensionType type) {
    
    std::vector<byte> buffer;
    std::vector<byte> magicString = FileManagementHelper::StringToBytes(DENIS_MAGIC_STRING);
    std::vector<byte> versionBytes = FileManagementHelper::IntToBytes(1, 1);
    std::vector<byte> formatBytes = FileManagementHelper::StringToBytes(EXTENSION_MAP.at(type));
    std::vector<byte> sizeBytes = FileManagementHelper::IntToBytes(static_cast<int>(data.size()), 8);

    buffer.insert(buffer.end(), magicString.begin(), magicString.end());        // magic string
    buffer.insert(buffer.end(), versionBytes.begin(), versionBytes.end());      // version of the file
    buffer.insert(buffer.end(), formatBytes.begin(), formatBytes.end());        // format of the data
    buffer.insert(buffer.end(), 7, 0x00);                                       // extra field
    buffer.insert(buffer.end(), sizeBytes.begin(), sizeBytes.end());            // size of the data

    if (buffer.size() != HEADER_LENGTH) {
        throw std::runtime_error("[e] Header length is incorrect: " + std::to_string(buffer.size()) +
                                 " instead of " + std::to_string(HEADER_LENGTH));
    }

    return buffer;

}

std::vector<byte> DenisEncoder::GetHeader(std::vector<byte> &data, DenisExtensionType type) {
    
    // get the header for the given version
    
    switch (GetVersion()) {
        case 1:
            return GetVersion1Header(data, type);
        default:
            throw std::invalid_argument("[e] Unsupported version :" + std::to_string(GetVersion()));
    }
}

