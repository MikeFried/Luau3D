#include "Config.h"
#include <iostream>
#include <filesystem>

Config::Config() : showHelp(false) {
    // Default script path is main.luau in current directory
    scriptPath = "main.luau";
}

Config::~Config() {}

bool Config::parseArguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            showHelp = true;
            return true;
        }
        else if (arg == "--run" || arg == "-r") {
            if (i + 1 < argc) {
                scriptPath = argv[++i];
            }
            else {
                std::cerr << "Error: --run requires a script path argument" << std::endl;
                return false;
            }
        }
        else {
            // If no recognized argument, treat it as a script path
            scriptPath = arg;
        }
    }

    // Check if the script file exists
    if (!std::filesystem::exists(scriptPath)) {
        std::cerr << "Error: Script file '" << scriptPath << "' not found" << std::endl;
        return false;
    }

    return true;
}

void Config::printHelp() const {
    std::cout << "Luau3D Engine - A 3D engine with Luau scripting\n\n";
    std::cout << "Usage: luau3d [options] [script]\n\n";
    std::cout << "Options:\n";
    std::cout << "  --help, -h           Show this help message\n";
    std::cout << "  --run <script>, -r   Run the specified script\n";
    std::cout << "\n";
    std::cout << "If no script is specified, the engine will attempt to run 'main.luau'\n";
    std::cout << "in the current working directory.\n";
} 