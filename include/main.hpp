#ifndef MAIN_HPP
#define MAIN_HPP


#include <iostream>
#include <string>
#include <filesystem>

#include "FormatManager/FileManagementHelper.hpp"
#include "FormatManager/DenisEncoder.hpp"
#include "FormatManager/DenisDecoder.hpp"
#include "FormatManager/DenisExtension.hpp"
#include "PhysicalStorage/MatrixChunker.h"
#include "PhysicalStorage/QRCodeStorage.hpp"
#include "PhysicalStorage/PBMUtils.h"


int main(int argc, char* argv[]);


#endif