#include "engine/Engine.h"
#include "engine/Config.h"
#include <iostream>
#include <filesystem>

int main(int argc, char* argv[]) {
    Config config;
    
    // Parse command line arguments
    if (!config.parseArguments(argc, argv)) {
        return 1;
    }

    // Show help and exit if requested
    if (config.shouldShowHelp()) {
        config.printHelp();
        return 0;
    }

    std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;
    
    Engine engine;

    // Initialize the engine with a window
    if (!engine.initialize("Luau3D Engine", 800, 600)) {
        std::cerr << "Failed to initialize engine" << std::endl;
        return 1;
    }

    // Load and execute the script
    if (!engine.loadScript(config.getScriptPath())) {
        std::cerr << "Failed to load script: " << config.getScriptPath() << std::endl;
        return 1;
    }

    // Run the main game loop
    engine.run();

    return 0;
} 