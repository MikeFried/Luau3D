#pragma once

#include <string>
#include <memory>
#include "GLRenderer.h"
#include "LuauBinding.h"
#include "ILuauModule.h"

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
    GLRenderer* getRenderer() { return renderer.get(); }

    // Get the Luau binding
    LuauBinding* getLuauBinding() { return luauBinding.get(); }

    // Register a module with the Luau binding
    void registerModule(const ILuauModule* module);

private:
    std::unique_ptr<GLRenderer> renderer;
    std::unique_ptr<LuauBinding> luauBinding;
    bool isRunning;
}; 