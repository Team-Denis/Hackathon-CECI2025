#pragma once

#include <array>
#include <optional>
#include <GL/glew.h>
#include <GL/gl.h>

constexpr int SIDE = 256;
constexpr int BUFFER_SIZE = SIDE * SIDE;

class GPUCellularAutomaton {
public:
    GPUCellularAutomaton();

    ~GPUCellularAutomaton();

    void runForward();

    void runBackward();

    void clearPrevGrid() const;

    void writeCurrGrid(std::array<GLint, BUFFER_SIZE> &currGrid) const;

    void writePrevGrid(std::array<GLint, BUFFER_SIZE> &prevGrid) const;

    void readCurrGrid(std::array<GLint, BUFFER_SIZE> &currGrid) const;

    void readPrevGrid(std::array<GLint, BUFFER_SIZE> &prevGrid) const;

private:
    unsigned int m_buffers[3];
    unsigned int m_forward_shader_program = 0;
    unsigned int m_backward_shader_program = 0;

    int m_prev_buffer = 0;
    int m_current_buffer = 1;
    int m_next_buffer = 2;
};
