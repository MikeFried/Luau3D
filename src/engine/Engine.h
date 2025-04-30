#pragma once

#include <string>
#include <memory>
#include "IRenderer.h"
#include "LuauBinding.h"
#include "ILuauModule.h"
#include "Luau3D.h"

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

    // Get the renderer
    IRenderer* getRenderer() { return renderer.get(); }

    // Get the Luau binding
    LuauBinding* getLuauBinding() { return luauBinding.get(); }

    // Register a module with the Luau binding
    void registerModule(const ILuauModule* module);

private:
    std::unique_ptr<IRenderer> renderer;
    std::unique_ptr<LuauBinding> luauBinding;
    std::unique_ptr<Luau3D> luau3d;
    bool isRunning;
}; 