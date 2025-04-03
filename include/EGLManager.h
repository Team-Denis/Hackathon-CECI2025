#pragma once

class EGLManager {
public:
    EGLManager() = default;

    static void init();

    static void cleanup();
};
