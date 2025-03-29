/*
* This data storage is inspired by the GitHub Arctic Vault : 
* https://archiveprogram.github.com/arctic-vault/
*/


#pragma once

#include "common.hpp"
#include <qrencode.h>

void write_qr_to_file(QRcode qr, std::string filename);
void data_chunk_to_qr(std::vector<uint8_t> chunk, int chunk_num);