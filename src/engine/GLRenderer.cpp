#include "GLRenderer.h"
#include <iostream>

// Window procedure callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

GLRenderer::GLRenderer() : width(0), height(0), hwnd(nullptr), hdc(nullptr), hrc(nullptr), windowOpen(true) {
    clearColor[0] = 0.0f;
    clearColor[1] = 0.0f;
    clearColor[2] = 0.0f;
    clearColor[3] = 1.0f;
}

GLRenderer::~GLRenderer() {
    if (hrc) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(hrc);
    }
    if (hwnd) {
        ReleaseDC(hwnd, hdc);
        DestroyWindow(hwnd);
    }
}

bool GLRenderer::initialize(const std::string& windowTitle, int w, int h) {
    width = w;
    height = h;

    // Register window class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "Luau3DWindow";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    
    if (!RegisterClassEx(&wc)) {
        std::cerr << "Failed to register window class" << std::endl;
        return false;
    }

    // Create window
    hwnd = CreateWindowEx(
        0,
        "Luau3DWindow",
        windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!hwnd) {
        std::cerr << "Failed to create window" << std::endl;
        return false;
    }

    // Get device context
    hdc = GetDC(hwnd);

    // Set up pixel format
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    pfd.cStencilBits = 8;
    pfd.iLayerType = PFD_MAIN_PLANE;

    int pixelFormat = ChoosePixelFormat(hdc, &pfd);
    if (!pixelFormat) {
        std::cerr << "Failed to choose pixel format" << std::endl;
        return false;
    }

    if (!SetPixelFormat(hdc, pixelFormat, &pfd)) {
        std::cerr << "Failed to set pixel format" << std::endl;
        return false;
    }

    // Create OpenGL context
    hrc = wglCreateContext(hdc);
    if (!hrc) {
        std::cerr << "Failed to create OpenGL context" << std::endl;
        return false;
    }

    if (!wglMakeCurrent(hdc, hrc)) {
        std::cerr << "Failed to make OpenGL context current" << std::endl;
        return false;
    }

    // Enable only point smoothing
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    // After OpenGL context is created and made current:
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-1, 1, -1, 1, 1, 100);

    // Reset to modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    ShowWindow(hwnd, SW_SHOW);
    return true;
}

void GLRenderer::beginFrame() {
    // Process window messages
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            windowOpen = false;
            return;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void GLRenderer::endFrame() {
    SwapBuffers(hdc);
}

void GLRenderer::clear() {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GLRenderer::setClearColor(float r, float g, float b, float a) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
}

bool GLRenderer::isWindowOpen() const {
    return windowOpen;
}

void GLRenderer::setGeometryColor(float r, float g, float b) {
    geometryColor[0] = r;
    geometryColor[1] = g;
    geometryColor[2] = b;
}

void GLRenderer::enableLighting(bool enable) {
    if (enable) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }
}
