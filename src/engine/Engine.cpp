#include "Engine.h"
#include <iostream>

Engine::Engine() : isRunning(false) {}

Engine::~Engine() {
    // Cleanup handled by unique_ptr
}

bool Engine::initialize(const std::string& windowTitle, int width, int height) {
    try {
        // Initialize renderer
        renderer = std::make_unique<GLRenderer>();
        if (!renderer->initialize(windowTitle, width, height)) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return false;
        }

        // Initialize Luau binding
        luauBinding = std::make_unique<LuauBinding>(renderer.get());
        if (!luauBinding->initialize()) {
            std::cerr << "Failed to initialize Luau binding" << std::endl;
            return false;
        }

        isRunning = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize engine: " << e.what() << std::endl;
        return false;
    }
}

void Engine::run() {
    while (isRunning) {
        // Execute any pending Luau code
        luauBinding->execute();

        // Render
        renderer->beginFrame();
        renderer->clear();
        renderer->endFrame();

        // Check if we should continue running
        isRunning = renderer->isWindowOpen();
    }
}

bool Engine::loadScript(const std::string& scriptPath) {
    if (!luauBinding->loadScript(scriptPath)) {
        std::cerr << "Failed to load script: " << scriptPath << std::endl;
        return false;
    }
    return true;
} 