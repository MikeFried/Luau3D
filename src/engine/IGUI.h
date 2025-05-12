#pragma once

#include "ILuauModule.h"
#include <string>

// Platform-agnostic window information
struct WindowInfo {
    void* handle;
    void* context;
    int width, height;
};

// Interface for the GUI
class IGUI : public ILuauModule {
public:
    virtual ~IGUI() = default;

    // Initialize the GUI
    virtual bool initialize(const std::string& windowTitle, int width, int height) = 0;

    // Check if window is still open
    virtual bool isWindowOpen() const = 0;

    // Pump messages
    virtual void pumpMessages() = 0;

    // Handle a key event
    virtual void handleKeyEvent(const std::string& key, const std::string& action) = 0;

    // Get window handle
    virtual WindowInfo getWindowInfo() const = 0;
}; 