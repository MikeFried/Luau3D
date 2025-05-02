#pragma once

#include "ILuauModule.h"
#include "IRenderer.h"
#include "LuauBinding.h"
#include "lua.h"
#include <functional>
#include <vector>

class GUI : public ILuauModule {
public:
    GUI(IRenderer* renderer, LuauBinding* luauBinding);
    ~GUI();

    // ILuauModule implementation
    const char* getModuleName() const override { return "gui.luau"; }
    const LuauExport* getExports() const override;

    // Helper to get the GUI instance from Lua state
    static GUI* getInstance(lua_State* L);

    // Keyboard callback handling
    void registerKeyboardCallback(int callbackRef);
    void handleKeyEvent(const char* key, const char* action);

private:
    IRenderer* renderer;
    LuauBinding* luauBinding;
    std::vector<int> keyboardCallbacks;  // References to Lua callback functions
};

// Global instance pointer for Lua functions
extern GUI* g_gui; 