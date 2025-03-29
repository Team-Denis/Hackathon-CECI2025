#include "../headers/storage.hpp"
#include "../headers/visualization.hpp"


int main(int argc, char const *argv[])
{
    std::vector<uint8_t> data = {0x64, 0x65, 0x6E, 0x69, 0x73};
    data_chunk_to_qr(data, 0);

    draw_qr("output/qr_code_0.pbm");

    return 0;
}
