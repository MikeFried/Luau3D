#include "LuauBinding.h"
#include "GLRenderer.h"
#include "lua.h"
#include "lualib.h"
#include "luaconf.h"
#include "luacode.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

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

        currentScriptPath = scriptPath;
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

// TODO: Add internal modules for OpenGL, file system, DLLImport
// Check if the module is an internal module

static int require(lua_State* L) {
    std::string modulePath = luaL_checkstring(L, 1);
    std::cout << "require called with path: " << modulePath << std::endl;
    
    // Try to load the module using the LuauBinding instance
    LuauBinding* binding = static_cast<LuauBinding*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!binding) {
        std::cerr << "Internal error: LuauBinding not found" << std::endl;
        luaL_error(L, "Internal error: LuauBinding not found");
        return 0;
    }

    if (!binding->loadModule(modulePath)) {
        std::cerr << "Failed to load module: " << modulePath << std::endl;
        luaL_error(L, "Failed to load module: %s", modulePath.c_str());
        return 0;
    }

    std::cout << "Module loaded successfully, stack size: " << lua_gettop(L) << std::endl;
    return 1;
}

// TODO: Check for relative path and add .luau or .lua to the end
// It should be a warning if the user didn't specify the exact file name
// to encourage good coding practices.

bool LuauBinding::loadModule(const std::string& modulePath) {
    try {
        if (currentScriptPath.empty()) {
            std::cerr << "No current script path available" << std::endl;
            return false;
        }

        std::filesystem::path scriptPath(currentScriptPath);
        std::filesystem::path scriptDir = scriptPath.parent_path();
        std::filesystem::path fullPath = scriptDir / modulePath;

        std::cout << "Loading module from: " << fullPath.string() << std::endl;

        // Load the module script
        if (!loadScript(fullPath.string())) {
            std::cerr << "Module not found: " << fullPath.string() << std::endl;
            return false;
        }

        std::cout << "Script loaded, executing module function..." << std::endl;

        // Execute the module function to get its result
        if (lua_pcall(L, 0, 1, 0) != 0) {
            std::cerr << "Failed to execute module: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
            return false;
        }

        std::cout << "Module executed successfully, stack size: " << lua_gettop(L) << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to load module: " << e.what() << std::endl;
        return false;
    }
}

void LuauBinding::registerBindings() {
    // TODO: Move these into an engine module - don't leave them free floating in the global scope
    // Register engine functions that can be called from Luau
    lua_pushcfunction(L, setClearColor, "setClearColor");
    lua_setglobal(L, "setClearColor");

    lua_pushcfunction(L, getDeltaTime, "getDeltaTime");
    lua_setglobal(L, "getDeltaTime");

    lua_pushcfunction(L, isRunning, "isRunning");
    lua_setglobal(L, "isRunning");

    lua_pushcfunction(L, present, "present");
    lua_setglobal(L, "present");

    lua_pushcfunction(L, drawGeometry, "drawGeometry");
    lua_setglobal(L, "drawGeometry");

    // Register loadModule function instead of require
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, require, "require", 1);
    lua_setglobal(L, "require");
} 