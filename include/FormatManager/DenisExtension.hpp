#ifndef DENIS_EXTENSION_HPP
#define DENIS_EXTENSION_HPP

#include <iostream>
#include <string>
#include <unordered_map>


typedef unsigned char byte;

enum class DenisExtensionType {
    ANY,
    NONE,
    TXT,
    IMG,
};


const std::unordered_map<DenisExtensionType, std::string> EXTENSION_MAP = {
    {DenisExtensionType::ANY, "ANY"},
    {DenisExtensionType::NONE, "NUL"},
    {DenisExtensionType::TXT, "TXT"},
    {DenisExtensionType::IMG, "IMG"},
};

const std::unordered_map<std::string, DenisExtensionType> EXTENSION_MAP_REVERSE = {
    {"ANY", DenisExtensionType::ANY},
    {"NUL", DenisExtensionType::NONE},
    {"TXT", DenisExtensionType::TXT},
    {"IMG", DenisExtensionType::IMG},
};


const int HEADER_LENGTH = 24;
const std::string DENIS_MAGIC_STRING = "DENIS";
const std::vector<byte> DENIS_TERMINATOR = std::vector<byte>(8, 0xFF);


#endif