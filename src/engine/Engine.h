#pragma once

#include <string>
#include <memory>
#include "IRenderer.h"
#include "LuauBinding.h"
#include "ILuauModule.h"
#include "Luau3D.h"
#include "IGUI.h"

// The main engine class
class Engine {
public:
    Engine();
    ~Engine();

    // Initialize the engine
    bool initialize(const std::string& windowTitle, int width, int height);

    // Run the main game loop
    void run();

    // Load and execute a Luau script
    bool loadScript(const std::string& scriptPath);

    // Register a module with the Luau binding
    void registerModule(const ILuauModule* module);

private:
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<LuauBinding> luauBinding;
    std::unique_ptr<Luau3D> luau3d;
    std::unique_ptr<IGUI> gui;
}; 