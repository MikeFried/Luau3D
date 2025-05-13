#include "Engine.h"
#include "Windows/GLRenderer.h"
#include "Windows/GUI.h"
#include <iostream>

Engine::Engine() {}

Engine::~Engine() {
    // Cleanup handled by unique_ptr
}

bool Engine::initialize(const std::string& windowTitle, int width, int height) {
    try {
        // Initialize Luau binding
        luauBinding = std::make_unique<LuauBinding>();
        if (!luauBinding->initialize()) {
            std::cerr << "Failed to initialize Luau binding" << std::endl;
            return false;
        }

        // Initialize modules
        gui = std::make_unique<GUI>(luauBinding.get());
        renderer = std::make_unique<GLRenderer>(gui.get());

        if (!gui->initialize(windowTitle, width, height)) {
            std::cerr << "Failed to initialize gui" << std::endl;
            return false;
        }

        if (!renderer->initialize()) {
            std::cerr << "Failed to initialize renderer" << std::endl;
            return false;
        }

        // Initialize Luau3D
        luau3d = std::make_unique<Luau3D>(gui.get(), renderer.get());

        // Register modules
        registerModule(luau3d.get());
        registerModule(gui.get());
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize engine: " << e.what() << std::endl;
        return false;
    }
}

void Engine::registerModule(const ILuauModule* module) {
    if (luauBinding && module) {
        luauBinding->registerInternalModule(module->getModuleName(), module->getExports());
    }
}

void Engine::run() {
    // Execute any pending Luau code
    luauBinding->execute();

    while (gui->isWindowOpen()) {
        luau3d->present(luauBinding->getLuaState());
    }
}

bool Engine::loadScript(const std::string& scriptPath) {
    if (!luauBinding->loadScript(scriptPath)) {
        std::cerr << "Failed to load script: " << scriptPath << std::endl;
        return false;
    }
    return true;
} 