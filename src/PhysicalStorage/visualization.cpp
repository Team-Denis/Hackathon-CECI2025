#include "visualization.hpp"


void draw_qr(std::string filename) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "QR Code");
    SetTargetFPS(60);

    std::ifstream file;
    file.open(filename);

    // skips first line
    std::string line;
    std::getline(file, line);

    // gets width and height ("width height\n")
    std::getline(file, line);
    int width = std::stoi(line.substr(0, line.find(" ")));
    int height = std::stoi(line.substr(line.find(" ") + 1));

    std::cout << width << " " << height << std::endl;

    // gets data
    std::vector<uint8_t> data;
    while (std::getline(file, line)) {
        for (int i = 0; i < width; i++) {
            data.push_back(std::stoi(line.substr(i * 2, 1)));
            std::cout << data[i] << " ";
        }
    }

    file.close();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                if (data[i + j * width] == 1) {
                    DrawRectangle(i * SCREEN_WIDTH / width, j * SCREEN_HEIGHT / height, SCREEN_WIDTH / width + 1, SCREEN_HEIGHT / height + 1, BLACK);
                }
            }
        }

        EndDrawing();
    }

    CloseWindow();
}