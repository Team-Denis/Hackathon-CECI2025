#include "CellularAutomatonVisualizer.hpp"
#include <raylib.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cmath>

CellularAutomatonVisualizer::CellularAutomatonVisualizer(
    const char *title,
    int width,
    int height
) : m_title(title),
    m_width(width),
    m_height(height),
    m_running(false),
    m_speed(0), // No delay
    m_paused(false),
    m_displayMode(0) // Show current grid only
{
}

CellularAutomatonVisualizer::~CellularAutomatonVisualizer() {
    stop();
}

void CellularAutomatonVisualizer::start() {
    if (!m_running.load()) {
        m_running.store(true);
        m_thread = std::thread(&CellularAutomatonVisualizer::visualizerThread, this);
    }
}

void CellularAutomatonVisualizer::stop() {
    if (m_running.load()) {
        m_running.store(false);
        m_cv.notify_all();
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }
}

void CellularAutomatonVisualizer::updateGridState(
    const std::array<int, BUFFER_SIZE> &currentGrid,
    const std::array<int, BUFFER_SIZE> &prevGrid,
    int chunkIndex,
    int totalChunks,
    int iteration,
    int totalIterations
) {
    GridState state;
    state.currentGrid = currentGrid;
    state.prevGrid = prevGrid;
    state.chunkIndex = chunkIndex;
    state.totalChunks = totalChunks;
    state.iteration = iteration;
    state.totalIterations = totalIterations; {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Keep queue from growing too large by removing oldest states if needed
        while (m_stateQueue.size() > 10) {
            m_stateQueue.pop();
        }
        m_stateQueue.push(state);
    }

    m_cv.notify_one();
}

bool CellularAutomatonVisualizer::isRunning() const {
    return m_running.load();
}

void CellularAutomatonVisualizer::setSpeed(int delayMs) {
    m_speed.store(std::max(0, delayMs));
}

void CellularAutomatonVisualizer::visualizerThread() {
    // Initialize raylib window
    SetTraceLogLevel(LOG_WARNING); // Reduce log verbosity
    InitWindow(m_width, m_height, m_title.c_str());
    SetTargetFPS(60);

    GridState currentState{};
    bool hasState = false;

    while (m_running.load() && !WindowShouldClose()) {
        // Only check if window should close

        // Always get next state if available
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if (!m_stateQueue.empty()) {
                currentState = m_stateQueue.front();
                m_stateQueue.pop();
                hasState = true;
            }
        }

        // Draw frame
        BeginDrawing();
        ClearBackground(WHITE);

        if (hasState) {
            // Always draw only the current grid, centered
            drawGrid(currentState.currentGrid, m_width / 4, m_height / 4, m_width / 2, m_height / 2);

            // Draw status information
            drawStatus(currentState);
        } else {
            // No data yet
            DrawText("Waiting for data...", 20, m_height / 2 - 20, 30, DARKGRAY);
        }

        // Draw title
        DrawText("Cellular Automaton State", 10, m_height - 30, 20, BLACK);

        EndDrawing();

        // Add delay for speed control
        if (m_speed.load() > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(m_speed.load()));
        }
    }

    CloseWindow();
    m_running.store(false);
}

void CellularAutomatonVisualizer::drawGrid(
    const std::array<int, BUFFER_SIZE> &grid,
    int x, int y, int width, int height
) {
    int gridSide = static_cast<int>(sqrt(BUFFER_SIZE));
    float cellWidth = static_cast<float>(width) / gridSide;
    float cellHeight = static_cast<float>(height) / gridSide;

    // Only draw cells that would be visible (optimization)
    int cellsDrawn = 0;
    const int maxVisibleCells = 10000; // Limit for performance

    for (int gy = 0; gy < gridSide && cellsDrawn < maxVisibleCells; gy++) {
        for (int gx = 0; gx < gridSide && cellsDrawn < maxVisibleCells; gx++) {
            int index = gy * gridSide + gx;
            if (grid[index]) {
                DrawRectangle(
                    x + static_cast<int>(gx * cellWidth),
                    y + static_cast<int>(gy * cellHeight),
                    std::max(1, static_cast<int>(cellWidth)),
                    std::max(1, static_cast<int>(cellHeight)),
                    BLACK
                );
                cellsDrawn++;
            }
        }
    }

    // Draw grid outline
    DrawRectangleLines(x, y, width, height, BLACK);

    // No need for grid label
}

void CellularAutomatonVisualizer::drawStatus(const GridState &state) {
    std::stringstream statusText;
    statusText << "Chunk: " << (state.chunkIndex + 1) << " / " << state.totalChunks;

    if (state.totalIterations > 0) {
        statusText << " | Iteration: " << state.iteration << " / " << state.totalIterations;
    }

    DrawText(statusText.str().c_str(), 20, 20, 20, BLACK);
}
