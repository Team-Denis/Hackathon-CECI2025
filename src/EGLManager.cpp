#include <EGLManager.h>
#include <GL/glew.h>
#include <EGL/egl.h>
#include <iostream>

EGLDisplay eglDpy;
EGLContext eglCtx;

void EGLManager::init() {
    // 1. Initialize EGL
    eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (eglDpy == EGL_NO_DISPLAY) {
        std::cerr << "EGL: Failed to get display" << std::endl;
        exit(1);
    }

    EGLint major, minor;
    if (!eglInitialize(eglDpy, &major, &minor)) {
        std::cerr << "EGL: Failed to initialize" << std::endl;
        exit(1);
    }

    // 2. Select an appropriate configuration
    EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
    };

    EGLConfig config;
    EGLint numConfigs;
    if (!eglChooseConfig(eglDpy, configAttribs, &config, 1, &numConfigs)) {
        std::cerr << "EGL: Failed to choose config" << std::endl;
        exit(1);
    }

    // 3. Create a dummy pbuffer surface
    EGLint pbufferAttribs[] = {
        EGL_WIDTH, 1,
        EGL_HEIGHT, 1,
        EGL_NONE,
    };

    EGLSurface surface = eglCreatePbufferSurface(eglDpy, config, pbufferAttribs);
    if (surface == EGL_NO_SURFACE) {
        std::cerr << "EGL: Failed to create pbuffer surface" << std::endl;
        exit(1);
    }

    // 4. Bind the API
    eglBindAPI(EGL_OPENGL_API);

    // 5. Create a context and make it current
    EGLint contextAttribs[] = {
        EGL_CONTEXT_MAJOR_VERSION, 4,
        EGL_CONTEXT_MINOR_VERSION, 3,  // Minimum for compute shaders
        EGL_CONTEXT_OPENGL_PROFILE_MASK, EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT,
        EGL_NONE
    };

    eglCtx = eglCreateContext(eglDpy, config, EGL_NO_CONTEXT, contextAttribs);
    if (eglCtx == EGL_NO_CONTEXT) {
        std::cerr << "EGL: Failed to create context" << std::endl;
        exit(1);
    }

    if (!eglMakeCurrent(eglDpy, surface, surface, eglCtx)) {
        std::cerr << "EGL: Failed to make context current" << std::endl;
        exit(1);
    }

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        exit(1);
    }
}

void EGLManager::cleanup() {
    eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDpy, eglCtx);
    eglTerminate(eglDpy);
}