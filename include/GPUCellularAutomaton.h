#pragma once

#include <array>
#include <optional>
#include <GL/glew.h>
#include <GL/gl.h>

constexpr int SIDE = 256;
constexpr int BUFFER_SIZE = SIDE * SIDE;

class GPUCellularAutomaton {
public:
    explicit GPUCellularAutomaton(std::array<GLint, BUFFER_SIZE> const& current_grid, std::optional<std::array<GLint, BUFFER_SIZE>> const& prev_grid = {});

    void run_forward();
    void run_backward();

    void read_current_grid(std::array<GLint, BUFFER_SIZE>& current_grid) const;
    void read_prev_grid(std::array<GLint, BUFFER_SIZE>& prev_grid) const;
private:
    unsigned int m_buffers[3];
    unsigned int m_forward_shader_program = 0;
    unsigned int m_backward_shader_program = 0;

    int m_prev_buffer = 0;
    int m_current_buffer = 1;
    int m_next_buffer = 2;
};