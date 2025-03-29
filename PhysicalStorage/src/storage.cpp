#include "../headers/storage.hpp"


void write_qr_to_file(QRcode qr, std::string filename) {
    std::ofstream file;
    file.open(filename);
    // Header
    file << "P1" << std::endl;
    file << qr.width << " " << qr.width << std::endl;
    
    // Data
    for (int i = 0; i < qr.width; i++) {
        for (int j = 0; j < qr.width; j++) {
            file << qr.data[i + j * qr.width];
        }
        file << std::endl;
    }

    file.close();
}

void data_chunk_to_qr(std::vector<uint8_t> chunk, int chunk_num) {
    std::string filename = "output/qr_code_" + std::to_string(chunk_num) + ".pbm";
    QRcode *qr = QRcode_encodeData(chunk.size(), chunk.data(), 0, QR_ECLEVEL_H);
    write_qr_to_file(*qr, filename);
    QRcode_free(qr);
}