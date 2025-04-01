#include "LuauBinding.h"
#include "GLRenderer.h"
#include "lua.h"
#include "lualib.h"
#include "luaconf.h"
#include "luacode.h"
#include <fstream>
#include <sstream>
#include <iostream>

// Store the renderer pointer for use in Lua functions
static GLRenderer* g_renderer = nullptr;

LuauBinding::LuauBinding(GLRenderer* renderer) : renderer(renderer), L(nullptr) {
    g_renderer = renderer;
}

LuauBinding::~LuauBinding() {
    if (L) {
        lua_close(L);
    }
    g_renderer = nullptr;
}

bool LuauBinding::initialize() {
    try {
        L = luaL_newstate();
        if (!L) {
            return false;
        }
        luaL_openlibs(L);
        registerBindings();
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize Luau VM: " << e.what() << std::endl;
        return false;
    }
}

bool LuauBinding::loadScript(const std::string& scriptPath) {
    try {
        std::ifstream file(scriptPath);
        if (!file.is_open()) {
            std::cerr << "Failed to open script file: " << scriptPath << std::endl;
            return false;
        }

        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string source = buffer.str();

        size_t bytecodeSize = 0;
        char* bytecode = luau_compile(source.c_str(), source.length(), nullptr, &bytecodeSize);
        
        if (!bytecode) {
            std::cerr << "Failed to compile script" << std::endl;
            return false;
        }

        int result = luau_load(L, scriptPath.c_str(), bytecode, bytecodeSize, 0);
        free(bytecode);

        if (result != 0) {
            std::cerr << "Failed to load script: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load script: " << e.what() << std::endl;
        return false;
    }
}

bool LuauBinding::execute() {
    try {
        if (lua_pcall(L, 0, 0, 0) != 0) {
            std::cerr << "Failed to execute script: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to execute script: " << e.what() << std::endl;
        return false;
    }
}

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

void LuauBinding::registerBindings() {
    // Register engine functions that can be called from Luau
    lua_pushcfunction(L, setClearColor, "setClearColor");
    lua_setglobal(L, "setClearColor");

    lua_pushcfunction(L, getDeltaTime, "getDeltaTime");
    lua_setglobal(L, "getDeltaTime");

    lua_pushcfunction(L, isRunning, "isRunning");
    lua_setglobal(L, "isRunning");

    lua_pushcfunction(L, present, "present");
    lua_setglobal(L, "present");
} 