#include "LuauBinding.h"
#include "lua.h"
#include "lualib.h"
#include "luaconf.h"
#include "luacode.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>

LuauBinding::LuauBinding() : L(nullptr) {
}

LuauBinding::~LuauBinding() {
    if (L) {
        // Clean up cached modules
        for (const auto& [path, ref] : moduleCache) {
            lua_unref(L, ref);
        }
        moduleCache.clear();
        
        lua_close(L);
    }
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

bool LuauBinding::loadAndCacheModule(lua_State *L, const std::string& modulePath) {
    // Check if module is already in cache
    auto it = moduleCache.find(modulePath);
    if (it != moduleCache.end()) {
        // Module is cached, push it onto the stack
        lua_rawgeti(L, LUA_REGISTRYINDEX, it->second);
        return true;
    }
    
    // Try to load the module
    if (!loadModule(modulePath)) {
        std::cerr << "Failed to load module: " << modulePath << std::endl;
        return false;
    }

    // Cache the module
    int ref = lua_ref(L, -1);
    moduleCache[modulePath] = ref;
    return true;
}

static int require(lua_State* L) {
    std::string modulePath = luaL_checkstring(L, 1);
    std::cout << "require called with path: " << modulePath << std::endl;
    
    // Get the LuauBinding instance
    LuauBinding* binding = static_cast<LuauBinding*>(lua_touserdata(L, lua_upvalueindex(1)));
    if (!binding) {
        luaL_error(L, "Internal error: LuauBinding not found");
        return 0;
    }

    if (!binding->loadAndCacheModule(L, modulePath)) {
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

void LuauBinding::makeTableForInternalModule(lua_State* L, const LuauExport exports[])
{
    lua_createtable(L, 0, sizeof(exports) / sizeof(exports[0]));
    for (int i = 0; exports[i].name != nullptr; i++)
    {
        lua_pushcfunction(L, exports[i].func, exports[i].name);
        lua_setfield(L, -2, exports[i].name);
    }
    lua_setreadonly(L, -1, 1);
}

void LuauBinding::registerBindings() {
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, require, "require", 1);
    lua_setglobal(L, "require");
}

void LuauBinding::registerInternalModule(const std::string& name, const LuauExport exports[]) {
    makeTableForInternalModule(L, exports);
    int ref = lua_ref(L, -1);
    moduleCache[name] = ref;
} 