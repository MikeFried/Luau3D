#pragma once

#include "ILuauModule.h"
#include "GLRenderer.h"

class Luau3D : public ILuauModule {
public:
    Luau3D(GLRenderer* renderer);
    ~Luau3D();

    // ILuauModule implementation
    const char* getModuleName() const override { return "luau3d.luau"; }
    const LuauExport* getExports() const override;

    // Static Lua binding functions
    static int setClearColor(lua_State* L);
    static int getDeltaTime(lua_State* L);
    static int isRunning(lua_State* L);
    static int present(lua_State* L);
    static int drawGeometry(lua_State* L);
    static int setLight(lua_State* L);

    // Helper to get the Luau3D instance from Lua state
    static Luau3D* getInstance(lua_State* L);

private:
    GLRenderer* renderer;
};
