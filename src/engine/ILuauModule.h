#pragma once

// Forward declare Lua state
struct lua_State;

// Type definition for Luau function exports
struct LuauExport {
    const char* name;
    int (*func)(lua_State* L);
};

// Interface for classes that want to export functions to Luau
class ILuauModule {
public:
    virtual ~ILuauModule() = default;
    
    // Get the module name
    virtual const char* getModuleName() const = 0;
    
    // Get the module exports
    virtual const LuauExport* getExports() const = 0;
}; 