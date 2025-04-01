#include "engine/Engine.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    
    Engine engine;

    // Initialize the engine with a window
    if (!engine.initialize("Luau3D Engine", 800, 600)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }

    // Load and execute the test script
    if (!engine.loadScript("scripts/test.lua")) {
        std::cerr << "Failed to load test script" << std::endl;
        return 1;
    }

    // Run the main game loop
    engine.run();

    return 0;
} 