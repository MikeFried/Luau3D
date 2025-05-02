#include "GLRenderer.h"
#include "GUI.h"
#include <iostream>

// TODO: Move all window processing logic to the GUI class
// Forward declare the GUI instance
extern GUI* g_gui;

// Window procedure callback
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_KEYDOWN:
        case WM_KEYUP:
            if (g_gui) {
                // Convert virtual key code to string
                char keyName[32];
                if (GetKeyNameTextA(lParam, keyName, sizeof(keyName)) > 0) {
                    // Convert to lowercase for consistency
                    for (char* p = keyName; *p; ++p) {
                        *p = tolower(*p);
                    }
                    // Forward the event to the GUI module
                    g_gui->handleKeyEvent(keyName, uMsg == WM_KEYDOWN ? "press" : "release");
                }
            }
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

void GLRenderer::enableLighting(bool enable) {
    if (enable) {
        glEnable(GL_LIGHTING);
    } else {
        glDisable(GL_LIGHTING);
    }
}

void GLRenderer::render(const std::vector<Model>& models) {
    // Enable vertex arrays for both position and color
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    
    // Set up the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    // Render all visible models
    for (const auto& model : models) {
        if (!model.visible) continue;
        
        // Apply CFrame transform
        glPushMatrix();
        
        // Translate to position
        glTranslatef(model.cframe.position[0], model.cframe.position[1], model.cframe.position[2]);
        
        // Create rotation matrix from look, up, and right vectors
        float matrix[16] = {
            model.cframe.right[0], model.cframe.up[0], -model.cframe.look[0], 0.0f,
            model.cframe.right[1], model.cframe.up[1], -model.cframe.look[1], 0.0f,
            model.cframe.right[2], model.cframe.up[2], -model.cframe.look[2], 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(matrix);
        
        const float* data = model.vertices.data();
        // Stride is 6 floats (3 for position, 3 for color)
        glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), data);
        // Color data starts 3 floats after each vertex
        glColorPointer(3, GL_FLOAT, 6 * sizeof(float), data + 3);
        
        glDrawArrays(GL_TRIANGLES, 0, model.vertices.size() / 6);
        
        glPopMatrix();
    }
    
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
}

void GLRenderer::setLight(int lightNum, const LightProperties& properties) {
    if (lightNum < 0 || lightNum > 7) return;
    GLenum light = GL_LIGHT0 + lightNum;
    
    // Helper function to set float array
    auto setFloatArray = [light](GLenum param, const std::vector<float>& values, int minSize, int maxSize) {
        if (values.size() >= minSize && values.size() <= maxSize) {
            float data[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            for (size_t i = 0; i < values.size(); i++) {
                data[i] = values[i];
            }
            glLightfv(light, param, data);
        }
    };
    
    // Set light properties
    if (!properties.position.empty()) {
        setFloatArray(GL_POSITION, properties.position, 3, 4);
    }
    if (!properties.ambient.empty()) {
        setFloatArray(GL_AMBIENT, properties.ambient, 3, 4);
    }
    if (!properties.diffuse.empty()) {
        setFloatArray(GL_DIFFUSE, properties.diffuse, 3, 4);
    }
    if (!properties.specular.empty()) {
        setFloatArray(GL_SPECULAR, properties.specular, 3, 4);
    }
    if (!properties.spotDirection.empty()) {
        setFloatArray(GL_SPOT_DIRECTION, properties.spotDirection, 3, 3);
    }
    
    if (properties.spotExponent >= 0.0f) {
        glLightf(light, GL_SPOT_EXPONENT, properties.spotExponent);
    }
    if (properties.spotCutoff >= 0.0f) {
        glLightf(light, GL_SPOT_CUTOFF, properties.spotCutoff);
    }
    if (properties.constantAttenuation >= 0.0f) {
        glLightf(light, GL_CONSTANT_ATTENUATION, properties.constantAttenuation);
    }
    if (properties.linearAttenuation >= 0.0f) {
        glLightf(light, GL_LINEAR_ATTENUATION, properties.linearAttenuation);
    }
    if (properties.quadraticAttenuation >= 0.0f) {
        glLightf(light, GL_QUADRATIC_ATTENUATION, properties.quadraticAttenuation);
    }
    
    glEnable(light);
}
