#include "GUI.h"
#define NOMINMAX
#include <windows.h>
#include "lua.h"
#include "lualib.h"
#include <iostream>

// Initialize global instance pointer
static GUI* g_gui = nullptr;

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
                    g_gui->handleKeyEvent(std::string(keyName), uMsg == WM_KEYDOWN ? "press" : "release");
                }
            }
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

GUI::GUI(LuauBinding* luauBinding) : luauBinding(luauBinding), width(0), height(0), hwnd(nullptr), hdc(nullptr), windowOpen(true) {
    g_gui = this;
}

GUI::~GUI() {
    g_gui = nullptr;
    if (hwnd) {
        ReleaseDC(hwnd, hdc);
        hdc = nullptr;
        DestroyWindow(hwnd);
        hwnd = nullptr;
    }
}

bool GUI::initialize(const std::string& windowTitle, int width, int height) {
    this->width = width;
    this->height = height;

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
    return true;
}

bool GUI::isWindowOpen() const {
    return windowOpen;
}

void GUI::pumpMessages() {
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

GUI* GUI::getInstance(lua_State* L) {
    if (!g_gui) {
        luaL_error(L, "GUI instance not initialized");
        return nullptr;
    }
    return g_gui;
}

// Register a keyboard callback from Lua
static int registerKeyboardCallback(lua_State* L) {
    GUI* instance = GUI::getInstance(L);
    if (!instance) return 0;

    // Check if we have a function as the first argument
    if (!lua_isfunction(L, 1)) {
        luaL_error(L, "Expected function as first argument");
        return 0;
    }

    // Store the callback function in the registry
    lua_pushvalue(L, 1);  // Push the function to the top
    int ref = lua_ref(L, -1);  // Reference the value at the top of the stack
    lua_pop(L, 1);  // Pop the function from the stack

    if (ref == LUA_NOREF) {
        luaL_error(L, "Failed to create reference to callback function");
        return 0;
    }

    // Store the reference
    instance->registerKeyboardCallback(ref);

    lua_pushboolean(L, 1);
    return 1;
}

void GUI::registerKeyboardCallback(int callbackRef) {
    keyboardCallbacks.push_back(callbackRef);
}

void GUI::handleKeyEvent(const std::string& key, const std::string& action) {
    lua_State* L = luauBinding->getLuaState();
    if (!L) return;

    // Call each registered callback
    for (int ref : keyboardCallbacks) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        lua_pushstring(L, key.c_str());
        lua_pushstring(L, action.c_str());
        
        if (lua_pcall(L, 2, 0, 0) != 0) {
            std::cerr << "Error in keyboard callback: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
        }
    }
}

// Define exports array
static LuauExport GuiExports[] = {
    {"registerKeyboardCallback", registerKeyboardCallback},
    {nullptr, nullptr}
};

const LuauExport* GUI::getExports() const {
    return GuiExports;
} 