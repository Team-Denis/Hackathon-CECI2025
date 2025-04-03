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

GPUCellularAutomaton::GPUCellularAutomaton() {
    m_forward_shader_program = load_compute_shader(reinterpret_cast<char *>(gol_forward_glsl), gol_forward_glsl_len);
    m_backward_shader_program = load_compute_shader(reinterpret_cast<char *>(gol_backward_glsl), gol_backward_glsl_len);
    glGenBuffers(3, m_buffers);

    // Initialize both buffers with the proper size
    for (unsigned int m_buffer : m_buffers) {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, BUFFER_SIZE * sizeof(GLint), nullptr, GL_DYNAMIC_COPY);
    }

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

GPUCellularAutomaton::~GPUCellularAutomaton() {
    glDeleteBuffers(3, m_buffers);
    glDeleteProgram(m_forward_shader_program);
    glDeleteProgram(m_backward_shader_program);
}

void GPUCellularAutomaton::runForward() {
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

void GPUCellularAutomaton::runBackward() {
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

void GPUCellularAutomaton::clearPrevGrid() const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_prev_buffer]);
    GLint* data = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    assert(data != nullptr);
    memset(data, 0, BUFFER_SIZE * sizeof(GLint));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUCellularAutomaton::writeCurrGrid(std::array<GLint, BUFFER_SIZE>& currGrid) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_current_buffer]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, currGrid.size() * sizeof(GLint), currGrid.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUCellularAutomaton::writePrevGrid(std::array<GLint, BUFFER_SIZE>& prev_grid) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_prev_buffer]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, prev_grid.size() * sizeof(GLint), prev_grid.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUCellularAutomaton::readCurrGrid(std::array<GLint, BUFFER_SIZE>& currGrid) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_current_buffer]);
    GLint* ptr = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (ptr) {
        memcpy(currGrid.data(), ptr, currGrid.size() * sizeof(GLint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void GPUCellularAutomaton::readPrevGrid(std::array<GLint, BUFFER_SIZE>& prevGrid) const {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_buffers[m_prev_buffer]);
    GLint* ptr = (GLint*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    if (ptr) {
        memcpy(prevGrid.data(), ptr, prevGrid.size() * sizeof(GLint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

