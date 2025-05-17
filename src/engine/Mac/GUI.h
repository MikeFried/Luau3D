#pragma once

#include <string>
#include <functional>
#include <vector>
#include "../ILuauModule.h"
#include "../IGUI.h"
#include "../IRenderer.h"
#include "../LuauBinding.h"
#include "lua.h"

class GUI : public IGUI {
public:
    GUI(LuauBinding* luauBinding);
    ~GUI();

    // Initialize the GUI
    bool initialize(const std::string& windowTitle, int width, int height) override;

    // Check if window is still open
    bool isWindowOpen() const override;
    void pumpMessages() override;
    
    // ILuauModule implementation
    const char* getModuleName() const override { return "gui.luau"; }
    const LuauExport* getExports() const override;

    // Helper to get the GUI instance from Lua state
    static GUI* getInstance(lua_State* L);

    // Keyboard callback handling
    void registerKeyboardCallback(int callbackRef);
    void handleKeyEvent(const std::string& key, const std::string& action) override;
    void onWindowClosed();

    // Get window handle
    WindowInfo getWindowInfo() const override;

private:
    LuauBinding* luauBinding;
    int width;
    int height;
    bool windowOpen;
    std::vector<int> keyboardCallbacks;  // References to Lua callback functions
    void* window;    // (NSWindow*) Cocoa window, used in GUI.mm
    void* delegate;  // (MacWindowDelegate*) Cocoa delegate, used in GUI.mm
}; 