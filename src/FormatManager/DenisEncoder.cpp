#include "FormatManager/DenisEncoder.hpp"

DenisEncoder::DenisEncoder(const int version) : version_(version) {
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

void DenisEncoder::Encode(std::string &fp, std::vector<byte> &data, DenisExtensionType type, int padding) {
    // encode the data and write it to the file
    if (type == DenisExtensionType::NONE) {
        throw std::invalid_argument("[e] Format cannot be NONE.");
    }

    std::vector<byte> bufferToWrite;
    std::vector<byte> header = GetHeader(data, type, padding);

    bufferToWrite.insert(bufferToWrite.end(), header.begin(), header.end());        // header
    bufferToWrite.insert(bufferToWrite.end(), data.begin(), data.end());            // data to write
    bufferToWrite.insert(bufferToWrite.end(), DENIS_TERMINATOR.begin(), DENIS_TERMINATOR.end()); // terminator

    // Open file in write mode (not append) to overwrite any existing content
    std::ofstream file(fp, std::ios::binary);
    if (!file) {
        throw std::runtime_error("[e] Error opening file for writing: " + fp);
    }

    file.write(reinterpret_cast<const char *>(bufferToWrite.data()), bufferToWrite.size());
    file.close();

    // Verify bytes written
    if (file_size(std::filesystem::path(fp)) != bufferToWrite.size()) {
        throw std::runtime_error("[e] Bytes written do not match buffer size.");
    }
}

std::vector<byte> DenisEncoder::GetVersion1Header(std::vector<byte> &data, DenisExtensionType type) {
    
    std::vector<byte> buffer;
    std::vector<byte> magicString = FileManagementHelper::StringToBytes(DENIS_MAGIC_STRING);
    std::vector<byte> versionBytes = FileManagementHelper::IntToBytes(1, 1);
    std::vector<byte> formatBytes = FileManagementHelper::StringToBytes(EXTENSION_MAP.at(type));
    std::vector<byte> sizeBytes = FileManagementHelper::IntToBytes(static_cast<int>(data.size()), 8);

    buffer.insert(buffer.end(), magicString.begin(), magicString.end());    // magic string
    buffer.insert(buffer.end(), versionBytes.begin(), versionBytes.end());  // version of the file
    buffer.insert(buffer.end(), formatBytes.begin(), formatBytes.end());    // format of the data
    buffer.insert(buffer.end(), 7, 0x00);                                       // extra field
    buffer.insert(buffer.end(), sizeBytes.begin(), sizeBytes.end());        // size of the data

    if (buffer.size() != HEADER_LENGTH) {
        throw std::runtime_error("[e] Header length is incorrect: " + std::to_string(buffer.size()) +
                                 " instead of " + std::to_string(HEADER_LENGTH));
    }

    return buffer;

}

std::vector<byte> DenisEncoder::GetVersion2Header(std::vector<byte> &data, DenisExtensionType type, int padding) {
    
    std::vector<byte> buffer;
    std::vector<byte> magicString = FileManagementHelper::StringToBytes(DENIS_MAGIC_STRING);
    std::vector<byte> versionBytes = FileManagementHelper::IntToBytes(2, 1);
    std::vector<byte> formatBytes = FileManagementHelper::StringToBytes(EXTENSION_MAP.at(type));
    std::vector<byte> paddingBytes = FileManagementHelper::IntToBytes(padding, 2);
    std::vector<byte> sizeBytes = FileManagementHelper::IntToBytes(static_cast<int>(data.size()), 8);

    buffer.insert(buffer.end(), magicString.begin(), magicString.end());    // magic string
    buffer.insert(buffer.end(), versionBytes.begin(), versionBytes.end());  // version of the file
    buffer.insert(buffer.end(), formatBytes.begin(), formatBytes.end());    // format of the data
    buffer.insert(buffer.end(), paddingBytes.begin(), paddingBytes.end());  // padding of the data
    buffer.insert(buffer.end(), 5, 0x00);                                   // extra field
    buffer.insert(buffer.end(), sizeBytes.begin(), sizeBytes.end());        // size of the data

    if (buffer.size() != HEADER_LENGTH) {
        throw std::runtime_error("[e] Header length is incorrect: " + std::to_string(buffer.size()) +
                                 " instead of " + std::to_string(HEADER_LENGTH));
    }

    return buffer;

}

std::vector<byte> DenisEncoder::GetHeader(std::vector<byte> &data, DenisExtensionType type, int padding) {
    // get the header for the given version
    switch (GetVersion()) {
        case 1:
            return GetVersion1Header(data, type);
        case 2:
            return GetVersion2Header(data, type, padding);
        default:
            throw std::invalid_argument("[e] Unsupported version :" + std::to_string(GetVersion()));
    }
}
