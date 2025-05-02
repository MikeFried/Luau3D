#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "lua.h"
#include "lualib.h"
#include "ILuauModule.h"

class LuauBinding {
public:
    LuauBinding();
    ~LuauBinding();

    // Initialize the Luau VM
    bool initialize();

    // Load and compile a script
    bool loadScript(const std::string& scriptPath);

    // Execute the loaded script
    bool execute();

    // Register C++ functions to be called from Luau
    void registerBindings();

    // Handle module loading and caching
    bool loadAndCacheModule(lua_State *L, const std::string& modulePath);

    // Load a module (internal or external)
    bool loadModule(const std::string& modulePath);

    // Register internal module exports
    void registerInternalModule(const std::string& name, const LuauExport exports[]);

    // Make a table for internal module exports
    void makeTableForInternalModule(lua_State *L, const LuauExport exports[]);

    // Get the Lua state
    lua_State* getLuaState() const { return L; }

private:
    lua_State* L;
    std::string currentScriptPath;
    std::unordered_map<std::string, int> moduleCache; // Cache of loaded modules by path
}; 