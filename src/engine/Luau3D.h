#pragma once

#include "ILuauModule.h"
#include "GLRenderer.h"
#include <vector>

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
    static int addModel(lua_State* L);
    static int removeModel(lua_State* L);
    static int clearModels(lua_State* L);
    static int setModelVisible(lua_State* L);
    static int updateModel(lua_State* L);
    static int setLight(lua_State* L);

    // Helper to get the Luau3D instance from Lua state
    static Luau3D* getInstance(lua_State* L);

    // Model management
    size_t addModel(const std::vector<float>& vertices, bool visible = true, const CFrame& cframe = CFrame());
    void removeModel(size_t index);
    void clearModels();
    void setModelVisible(size_t index, bool visible);
    void updateModel(size_t index, const std::vector<float>& vertices, bool visible, const CFrame& cframe);

private:
    GLRenderer* renderer;
    std::vector<Model> models;
};
