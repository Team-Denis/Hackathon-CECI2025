#ifndef DENIS_EXTENSION_HPP
#define DENIS_EXTENSION_HPP


#include <iostream>
#include <string>
#include <unordered_map>


enum class DenisExtensionType {
    NONE,
    TXT,
    IMG,
};


const std::unordered_map<DenisExtensionType, std::string> EXTENSION_MAP = {
    {DenisExtensionType::NONE, "NUL"},
    {DenisExtensionType::TXT, "TXT"},
    {DenisExtensionType::IMG, "IMG"},
};

const std::unordered_map<std::string, DenisExtensionType> EXTENSION_MAP_REVERSE = {
    {"NUL", DenisExtensionType::NONE},
    {"TXT", DenisExtensionType::TXT},
    {"IMG", DenisExtensionType::IMG},
};


const int HEADER_LENGTH = 24;
const std::string DENIS_MAGIC_STRING = "DENIS";




#endif