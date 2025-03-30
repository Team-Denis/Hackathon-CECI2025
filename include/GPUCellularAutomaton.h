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

    void run_forward();
    void run_backward();

    void clear_prev_grid() const;
    
    void upload_current_grid(std::array<GLint, BUFFER_SIZE>& current_grid) const;
    void upload_prev_grid(std::array<GLint, BUFFER_SIZE>& current_prev) const;

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