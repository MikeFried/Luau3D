#include "GLRenderer.h"
#include "LuauBinding.h"
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <vector>

// Global renderer pointer for Lua functions
static GLRenderer* g_renderer = nullptr;

// Implementation of the Luau3d functions
static int setClearColor(lua_State* L) {
    if (!g_renderer) return 0;
    
    float r = static_cast<float>(lua_tonumber(L, 1));
    float g = static_cast<float>(lua_tonumber(L, 2));
    float b = static_cast<float>(lua_tonumber(L, 3));
    float a = static_cast<float>(lua_tonumber(L, 4));
    
    g_renderer->setClearColor(r, g, b, a);
    return 0;
}

static int getDeltaTime(lua_State* L) {
    // For now, return a fixed delta time
    lua_pushnumber(L, 1.0 / 60.0);
    return 1;
}

static int isRunning(lua_State* L) {
    if (!g_renderer) {
        lua_pushboolean(L, false);
        return 1;
    }
    lua_pushboolean(L, g_renderer->isWindowOpen());
    return 1;
}

static int present(lua_State* L) {
    if (!g_renderer) return 0;
    
    g_renderer->beginFrame();
    g_renderer->clear();
    g_renderer->endFrame();
    return 0;
}

static int drawGeometry(lua_State* L) {
    if (!g_renderer) return 0;
    
    // Get the vertex data from the Lua table
    luaL_checktype(L, 1, LUA_TTABLE);
    int len = lua_objlen(L, 1);
    
    // Create a vector to store the vertex data
    std::vector<float> vertices;
    vertices.reserve(len);
    
    // Read the vertex data from the Lua table
    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, 1, i);
        vertices.push_back(static_cast<float>(lua_tonumber(L, -1)));
        lua_pop(L, 1);
    }
    
    // TODO: Implement actual OpenGL rendering in GLRenderer
    // For now, just print that we got the data
    //std::cout << "Drawing geometry with " << vertices.size() << " vertices" << std::endl;
    
    return 0;
}

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
    g_renderer = this;
}

GLRenderer::~GLRenderer() {
    if (g_renderer == this) {
        g_renderer = nullptr;
    }
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

static LuauExport Luau3dExports[] = {
    {"setClearColor", setClearColor},
    {"getDeltaTime", getDeltaTime},
    {"isRunning", isRunning},
    {"present", present},
    {"drawGeometry", drawGeometry},
    {nullptr, nullptr}
};

const LuauExport* GLRenderer::getExports() const {
    return Luau3dExports;
}
