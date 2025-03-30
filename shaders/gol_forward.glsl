#version 430 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(std430, binding = 0) buffer PrevState { int prev_grid[]; };  // S(t-1)
layout(std430, binding = 1) buffer CurrentState { int current_grid[]; }; // S(t)
layout(std430, binding = 2) buffer NextState { int next_grid[]; }; // S(t+1)

const int gridSize = 256;

int getCell(int x, int y) {
    if (x < 0 || x >= gridSize || y < 0 || y >= gridSize) return 0;
    return current_grid[y * gridSize + x];
}

void main() {
    int x = int(gl_GlobalInvocationID.x);
    int y = int(gl_GlobalInvocationID.y);
    if (x >= gridSize || y >= gridSize) return;

    int index = y * gridSize + x;

    // Count live neighbors
    int neighbors = getCell(x-1, y-1)
                  + getCell(x, y-1)
                  + getCell(x+1, y-1)
                  + getCell(x-1, y)
                  + getCell(x+1, y)
                  + getCell(x-1, y+1)
                  + getCell(x, y+1)
                  + getCell(x+1, y+1);

    // Game of Life rules
    int newState = (current_grid[index] == 1) ?
    ((neighbors == 2 || neighbors == 3) ? 1 : 0) :
    ((neighbors == 3) ? 1 : 0);

    next_grid[index] = newState ^ prev_grid[index];
}
