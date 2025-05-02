#include "GUI.h"
#include "lua.h"
#include "lualib.h"
#include <iostream>

// Initialize global instance pointer
GUI* g_gui = nullptr;

GUI::GUI(IRenderer* renderer, LuauBinding* luauBinding) 
    : renderer(renderer), luauBinding(luauBinding) {
    g_gui = this;
}

GUI::~GUI() {
    if (g_gui == this) {
        g_gui = nullptr;
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

void GUI::handleKeyEvent(const char* key, const char* action) {
    lua_State* L = luauBinding->getLuaState();
    if (!L) return;

    // Call each registered callback
    for (int ref : keyboardCallbacks) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, ref);
        lua_pushstring(L, key);
        lua_pushstring(L, action);
        
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