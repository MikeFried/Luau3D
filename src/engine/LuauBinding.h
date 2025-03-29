#pragma once

#include <string>
#include <memory>
#include "Luau/Common.h"
#include "Luau/Vm.h"
#include "Luau/Compiler.h"

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

private:
    GLRenderer* renderer;
    std::unique_ptr<Luau::Allocator> allocator;
    std::unique_ptr<Luau::AstNameTable> names;
    std::unique_ptr<Luau::BytecodeBuilder> bytecode;
    std::unique_ptr<Luau::VM> vm;
}; 