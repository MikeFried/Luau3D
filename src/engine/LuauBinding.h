#pragma once

#include <string>
#include <memory>
#include "lua.h"
#include "lualib.h"

class GLRenderer;

class LuauBinding {
public:
    LuauBinding(GLRenderer* renderer);
    ~LuauBinding();

    // Initialize the Luau VM
    bool initialize();

    // Load and compile a script
    bool loadScript(const std::string& scriptPath);

    // Execute the loaded script
    bool execute();

    // Register C++ functions to be called from Luau
    void registerBindings();

    // Load a module (internal or external)
    bool loadModule(const std::string& modulePath);

private:
    GLRenderer* renderer;
    lua_State* L;
    std::string currentScriptPath;
}; 