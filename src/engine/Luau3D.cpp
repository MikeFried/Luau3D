#include "Luau3D.h"
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <vector>

// Global instance pointer for Lua functions
static Luau3D* g_luau3d = nullptr;

Luau3D::Luau3D(GLRenderer* renderer) : renderer(renderer) {
    g_luau3d = this;
}

Luau3D::~Luau3D() {
    if (g_luau3d == this) {
        g_luau3d = nullptr;
    }
}

Luau3D* Luau3D::getInstance(lua_State* L) {
    if (!g_luau3d) {
        luaL_error(L, "Luau3D instance not initialized");
        return nullptr;
    }
    return g_luau3d;
}

int Luau3D::setClearColor(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    float r = static_cast<float>(lua_tonumber(L, 1));
    float g = static_cast<float>(lua_tonumber(L, 2));
    float b = static_cast<float>(lua_tonumber(L, 3));
    float a = static_cast<float>(lua_tonumber(L, 4));
    
    instance->renderer->setClearColor(r, g, b, a);
    return 0;
}

int Luau3D::getDeltaTime(lua_State* L) {
    // For now, return a fixed delta time
    lua_pushnumber(L, 1.0 / 60.0);
    return 1;
}

int Luau3D::isRunning(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) {
        lua_pushboolean(L, false);
        return 1;
    }
    lua_pushboolean(L, instance->renderer->isWindowOpen());
    return 1;
}

int Luau3D::present(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    instance->renderer->beginFrame();
    instance->renderer->clear();
    instance->renderer->endFrame();
    return 0;
}

int Luau3D::drawGeometry(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    // Get the vertex data from the Lua table
    luaL_checktype(L, 1, LUA_TTABLE);
    int len = lua_objlen(L, 1);
    
    // Create a vector to store the vertex data
    std::vector<float> vertices;
    vertices.reserve(len);
    
    // Read the vertex data from the Lua table
    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, 1, i);
        vertices.push_back(static_cast<float>(lua_tonumber(L, -1)));
        lua_pop(L, 1);
    }
    
    // TODO: Implement actual OpenGL rendering in GLRenderer
    // For now, just print that we got the data
    //std::cout << "Drawing geometry with " << vertices.size() << " vertices" << std::endl;
    
    return 0;
}

static LuauExport Luau3dExports[] = {
    {"setClearColor", Luau3D::setClearColor},
    {"getDeltaTime", Luau3D::getDeltaTime},
    {"isRunning", Luau3D::isRunning},
    {"present", Luau3D::present},
    {"drawGeometry", Luau3D::drawGeometry},
    {nullptr, nullptr}
};

const LuauExport* Luau3D::getExports() const {
    return Luau3dExports;
} 