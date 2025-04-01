#ifndef CELLULAR_AUTOMATON_VISUALIZER_HPP
#define CELLULAR_AUTOMATON_VISUALIZER_HPP

#include <string>
#include <vector>
#include <array>
#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <GPUCellularAutomaton.h>
#include <queue>

class CellularAutomatonVisualizer {
public:
    CellularAutomatonVisualizer(
        const char *title = "Cellular Automaton Visualizer",
        int width = 1024,
        int height = 800
    );

    ~CellularAutomatonVisualizer();

    // Start the visualizer in a separate thread
    void start();

    // Stop the visualizer thread
    void stop();

    // Add a new grid state to visualize (thread-safe)
    void updateGridState(
        const std::array<int, BUFFER_SIZE> &currentGrid,
        const std::array<int, BUFFER_SIZE> &prevGrid,
        int chunkIndex,
        int totalChunks,
        int iteration,
        int totalIterations
    );

    // Check if visualizer is running
    bool isRunning() const;

    // Set visualization speed (delay between frames in ms)
    void setSpeed(int delayMs);

private:
    // Structure to hold grid state information
    struct GridState {
        std::array<int, BUFFER_SIZE> currentGrid;
        std::array<int, BUFFER_SIZE> prevGrid;
        int chunkIndex;
        int totalChunks;
        int iteration;
        int totalIterations;
    };

    // Thread function that runs the visualization
    void visualizerThread();

    // Draw a single grid to the screen
    void drawGrid(const std::array<int, BUFFER_SIZE> &grid, int x, int y, int width, int height);

    // Draw status information
    void drawStatus(const GridState &state);

    // Window properties
    std::string m_title;
    int m_width;
    int m_height;

    // Thread synchronization
    std::thread m_thread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::queue<GridState> m_stateQueue;
    std::atomic<bool> m_running;
    std::atomic<int> m_speed; // Delay in milliseconds

    // Visualization state
    std::atomic<bool> m_paused;
    std::atomic<int> m_displayMode; // 0=current, 1=prev, 2=both
};

#endif // CELLULAR_AUTOMATON_VISUALIZER_HPP
