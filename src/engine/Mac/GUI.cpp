#include "GUI.h"
#include <iostream>

// Initialize global instance pointer
static GUI* g_gui = nullptr;

GUI::GUI(LuauBinding* luauBinding) 
    : luauBinding(luauBinding), width(800), height(600), windowOpen(false) {
    g_gui = this;
    std::cout << "[Mac] GUI constructed" << std::endl;
}

GUI::~GUI() {
    g_gui = nullptr;
    std::cout << "[Mac] GUI destroyed" << std::endl;
}

bool GUI::initialize(const std::string& windowTitle, int width, int height) {
    this->width = width;
    this->height = height;
    windowOpen = true;
    std::cout << "[Mac] GUI initialized with window: " << windowTitle 
              << " (" << width << "x" << height << ")" << std::endl;
    return true;
}

// This hack prevents an infinite loop because we only have stubs now.
static int numChecks = 5;
bool GUI::isWindowOpen() const {
    numChecks--;
    if (numChecks <= 0) {
        return false;
    }
    return windowOpen;
}

void GUI::pumpMessages() {
    std::cout << "[Mac] Pumping messages" << std::endl;
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
    std::cout << "[Mac] Registered keyboard callback: " << callbackRef << std::endl;
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
    std::cout << "[Mac] Key event: " << key << " - " << action << std::endl;
}

// Define exports array
static LuauExport GuiExports[] = {
    {"registerKeyboardCallback", registerKeyboardCallback},
    {nullptr, nullptr}
};

const LuauExport* GUI::getExports() const {
    return GuiExports;
}

WindowInfo GUI::getWindowInfo() const {
    WindowInfo info;
    info.handle = nullptr;
    info.context = nullptr;
    info.width = width;
    info.height = height;
    return info;
} 