#include <GPUCellularAutomaton.h>
#include <iostream>
#include <cassert>
#include <cstring>
#include "../shaders/gol_backward.h"
#include "../shaders/gol_forward.h"

unsigned int load_compute_shader(const char* source, int len) {
    unsigned int compute_shader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute_shader, 1, &source, &len);
    glCompileShader(compute_shader);

    int success;
    char infoLog[512];
    glGetShaderiv(compute_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(compute_shader, 512, nullptr, infoLog);
        std::cerr << "Compute Shader Compilation Error:\n" << infoLog << std::endl;
        exit(1);
    }

    unsigned int compute_program = glCreateProgram();
    glAttachShader(compute_program, compute_shader);
    glLinkProgram(compute_program);

    glGetProgramiv(compute_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(compute_program, 512, nullptr, infoLog);
        std::cerr << "Compute Shader Program Linking Error:\n" << infoLog << std::endl;
        exit(1);
    }

    glDeleteShader(compute_shader);

    return compute_program;
}

GPUCellularAutomaton::GPUCellularAutomaton(std::array<GLint, BUFFER_SIZE> const& current_grid, std::optional<std::array<GLint, BUFFER_SIZE>> const& prev_grid = {}) {
    m_forward_shader_program = load_compute_shader(reinterpret_cast<char *>(gol_forward_glsl), gol_forward_glsl_len);
    m_backward_shader_program = load_compute_shader(reinterpret_cast<char *>(gol_backward_glsl), gol_backward_glsl_len);
    glGenBuffers(3, m_buffers);

    // Initialize both buffers with the proper size
    for (unsigned int m_buffer : m_buffers) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, BUFFER_SIZE * sizeof(GLint), nullptr, GL_DYNAMIC_COPY);
    }

    // Set up initial state in the first buffer

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_current_buffer]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, current_grid.size() * sizeof(GLint), current_grid.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    if (prev_grid.has_value()) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_current_buffer]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, prev_grid.value().size() * sizeof(GLint), prev_grid.value().data(), GL_STATIC_DRAW);
    } else {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_prev_buffer]);
        GLint* data = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
        assert(data != nullptr);
        memset(data, 0, BUFFER_SIZE * sizeof(GLint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUCellularAutomaton::run_forward() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_buffers[m_prev_buffer]);  // S(t-1)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_buffers[m_current_buffer]); // S(t)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_buffers[m_next_buffer]); // S(t+1)

    glUseProgram(m_forward_shader_program);

    glDispatchCompute(SIDE / 16, SIDE / 16, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Rotate buffers: (prev -> current, current -> next, next -> prev)
    std::swap(m_prev_buffer, m_current_buffer);
    std::swap(m_current_buffer, m_next_buffer);
}

void GPUCellularAutomaton::run_backward() {
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_buffers[m_prev_buffer]);  // S(t-1)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_buffers[m_current_buffer]); // S(t)
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_buffers[m_next_buffer]); // S(t+1)

    glUseProgram(m_backward_shader_program);

    glDispatchCompute(SIDE / 16, SIDE / 16, 1);

    glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    // Rotate buffers: (prev <- current, current <- next, next <- prev)
    std::swap(m_current_buffer, m_next_buffer);
    std::swap(m_prev_buffer, m_current_buffer);
}

void GPUCellularAutomaton::read_current_grid(std::array<GLint, BUFFER_SIZE>& current_grid) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_current_buffer]);
    GLint* ptr = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (ptr) {
        memcpy(current_grid.data(), ptr, current_grid.size() * sizeof(GLint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUCellularAutomaton::read_prev_grid(std::array<GLint, BUFFER_SIZE>& prev_grid) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_current_buffer]);
    GLint* ptr = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (ptr) {
        memcpy(prev_grid.data(), ptr, prev_grid.size() * sizeof(GLint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

