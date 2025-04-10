#pragma once

#include <string>

class Config {
public:
    Config();
    ~Config();

    // Parse command line arguments
    bool parseArguments(int argc, char* argv[]);

    // Get configuration values
    const std::string& getScriptPath() const { return scriptPath; }
    bool shouldShowHelp() const { return showHelp; }

    // Print help message
    void printHelp() const;

private:
    std::string scriptPath;
    bool showHelp;
}; 