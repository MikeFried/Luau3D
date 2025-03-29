#include "engine/Engine.h"
#include <iostream>

int main(int argc, char* argv[]) {
    Engine engine;

    // Initialize the engine with a window
    if (!engine.initialize("Luau3D Engine", 800, 600)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }

    // Load and execute the main game script
    if (!engine.loadScript("scripts/main.luau")) {
        std::cerr << "Failed to load main script" << std::endl;
        return 1;
    }

    // Run the main game loop
    engine.run();

    return 0;
} 