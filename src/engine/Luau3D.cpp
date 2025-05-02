#include "Luau3D.h"
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>

// Global instance pointer for Lua functions
static Luau3D* g_luau3d = nullptr;

Luau3D::Luau3D(IRenderer* renderer) : renderer(renderer), beforeRenderCallbackRef(LUA_NOREF) {
    g_luau3d = this;
    lastDeltaTime = std::chrono::steady_clock::now();
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
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    auto now = std::chrono::steady_clock::now();
    auto delta = std::chrono::duration_cast<std::chrono::milliseconds>(now - instance->lastDeltaTime);
    instance->lastDeltaTime = now;
    lua_pushnumber(L, delta.count() / 1000.0);
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
    instance->callBeforeRenderCallback(L);
    instance->renderer->render(instance->models);
    instance->renderer->endFrame();
    
    return 0;
}

int Luau3D::addModel(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    // Get the model properties table
    luaL_checktype(L, 1, LUA_TTABLE);
    
    // Get vertices
    lua_getfield(L, 1, "vertices");
    luaL_checktype(L, -1, LUA_TTABLE);
    int len = lua_objlen(L, -1);
    
    std::vector<float> vertices;
    vertices.reserve(len);
    
    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, -1, i);
        vertices.push_back(static_cast<float>(lua_tonumber(L, -1)));
        lua_pop(L, 1);
    }
    lua_pop(L, 1); // Pop vertices table
    
    // Get visibility (optional)
    bool visible = true;
    lua_getfield(L, 1, "visible");
    if (lua_isboolean(L, -1)) {
        visible = lua_toboolean(L, -1) != 0;
    }
    lua_pop(L, 1);
    
    // Get CFrame (optional)
    CFrame cframe;
    lua_getfield(L, 1, "cframe");
    if (lua_istable(L, -1)) {
        // Helper function to get vector from table field
        auto getVector = [L](const char* field, float* vec) {
            lua_getfield(L, -1, field);
            if (lua_istable(L, -1)) {
                for (int i = 0; i < 3; i++) {
                    lua_rawgeti(L, -1, i + 1);
                    vec[i] = static_cast<float>(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);
        };
        
        getVector("position", cframe.position);
        getVector("look", cframe.look);
        getVector("up", cframe.up);
        getVector("right", cframe.right);
    }
    lua_pop(L, 1);
    
    // Add the model and return its index
    size_t index = instance->addModel(vertices, visible, cframe);
    lua_pushinteger(L, static_cast<lua_Integer>(index));
    return 1;
}

int Luau3D::removeModel(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    size_t index = static_cast<size_t>(luaL_checkinteger(L, 1));
    instance->removeModel(index);
    return 0;
}

int Luau3D::clearModels(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    instance->clearModels();
    return 0;
}

int Luau3D::setModelVisible(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    size_t index = static_cast<size_t>(luaL_checkinteger(L, 1));
    bool visible = lua_toboolean(L, 2) != 0;
    instance->setModelVisible(index, visible);
    return 0;
}

int Luau3D::updateModel(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    // Get model index
    size_t index = static_cast<size_t>(luaL_checkinteger(L, 1));
    
    // Get the model properties table
    luaL_checktype(L, 2, LUA_TTABLE);
    
    // Get vertices
    lua_getfield(L, 2, "vertices");
    luaL_checktype(L, -1, LUA_TTABLE);
    int len = lua_objlen(L, -1);
    
    std::vector<float> vertices;
    vertices.reserve(len);
    
    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, -1, i);
        vertices.push_back(static_cast<float>(lua_tonumber(L, -1)));
        lua_pop(L, 1);
    }
    lua_pop(L, 1); // Pop vertices table
    
    // Get visibility (optional)
    bool visible = true;
    lua_getfield(L, 2, "visible");
    if (lua_isboolean(L, -1)) {
        visible = lua_toboolean(L, -1) != 0;
    }
    lua_pop(L, 1);
    
    // Get CFrame (optional)
    CFrame cframe;
    lua_getfield(L, 2, "cframe");
    if (lua_istable(L, -1)) {
        // Helper function to get vector from table field
        auto getVector = [L](const char* field, float* vec) {
            lua_getfield(L, -1, field);
            if (lua_istable(L, -1)) {
                for (int i = 0; i < 3; i++) {
                    lua_rawgeti(L, -1, i + 1);
                    vec[i] = static_cast<float>(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                }
            }
            lua_pop(L, 1);
        };
        
        getVector("position", cframe.position);
        getVector("look", cframe.look);
        getVector("up", cframe.up);
        getVector("right", cframe.right);
    }
    lua_pop(L, 1);
    
    // Update the model
    instance->updateModel(index, vertices, visible, cframe);
    return 0;
}

int Luau3D::setLight(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    // Get light number (1-8)
    int lightNum = luaL_checkinteger(L, 1) - 1;
    if (lightNum < 0 || lightNum > 7) {
        luaL_error(L, "Light number must be between 1 and 8");
        return 0;
    }
    
    // Check properties table
    luaL_checktype(L, 2, LUA_TTABLE);
    
    LightProperties properties;
    
    // Helper function to get float array from table field
    auto getFloatArray = [L](const char* field) -> std::vector<float> {
        std::vector<float> result;
        lua_getfield(L, 2, field);
        if (lua_istable(L, -1)) {
            int size = lua_objlen(L, -1);
            result.reserve(size);
            for (int i = 0; i < size; i++) {
                lua_rawgeti(L, -1, i + 1);
                result.push_back(static_cast<float>(lua_tonumber(L, -1)));
                lua_pop(L, 1);
            }
        }
        lua_pop(L, 1);
        return result;
    };
    
    // Helper function to get single float from table field
    auto getFloat = [L](const char* field, float defaultVal) -> float {
        lua_getfield(L, 2, field);
        float result = lua_isnumber(L, -1) ? static_cast<float>(lua_tonumber(L, -1)) : defaultVal;
        lua_pop(L, 1);
        return result;
    };
    
    // Get light properties
    properties.position = getFloatArray("position");
    properties.ambient = getFloatArray("ambient");
    properties.diffuse = getFloatArray("diffuse");
    properties.specular = getFloatArray("specular");
    properties.spotDirection = getFloatArray("spotDirection");
    properties.spotExponent = getFloat("spotExponent", -1.0f);
    properties.spotCutoff = getFloat("spotCutoff", -1.0f);
    properties.constantAttenuation = getFloat("constantAttenuation", -1.0f);
    properties.linearAttenuation = getFloat("linearAttenuation", -1.0f);
    properties.quadraticAttenuation = getFloat("quadraticAttenuation", -1.0f);
    
    // Set the light properties
    instance->renderer->setLight(lightNum, properties);
    
    lua_pushboolean(L, 1);
    return 1;
}

int Luau3D::registerBeforeRenderCallback(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    // Check if we have a function as the first argument
    if (!lua_isfunction(L, 1)) {
        luaL_error(L, "Expected function as first argument");
        return 0;
    }
    
    // Store the callback function in the registry
    lua_pushvalue(L, 1);  // Push the function to the top
    int ref = lua_ref(L, -1);  // Reference the value at the top of the stack
    lua_pop(L, 1);  // Pop the function from the stack
    
    if (ref == LUA_NOREF) {
        luaL_error(L, "Failed to create reference to callback function");
        return 0;
    }
    
    // Store the reference
    instance->beforeRenderCallbackRef = ref;
    
    lua_pushboolean(L, 1);
    return 1;
}

void Luau3D::callBeforeRenderCallback(lua_State* L) {
    if (beforeRenderCallbackRef != LUA_NOREF) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, beforeRenderCallbackRef);
        if (lua_pcall(L, 0, 0, 0) != 0) {
            std::cerr << "Error in beforeRender callback: " << lua_tostring(L, -1) << std::endl;
            lua_pop(L, 1);
        }
    }
}

// Model management implementation
size_t Luau3D::addModel(const std::vector<float>& vertices, bool visible, const CFrame& cframe) {
    Model model;
    model.vertices = vertices;
    model.visible = visible;
    model.cframe = cframe;
    models.push_back(model);
    return models.size() - 1;
}

void Luau3D::removeModel(size_t index) {
    if (index < models.size()) {
        models.erase(models.begin() + index);
    }
}

void Luau3D::clearModels() {
    models.clear();
}

void Luau3D::setModelVisible(size_t index, bool visible) {
    if (index < models.size()) {
        models[index].visible = visible;
    }
}

void Luau3D::updateModel(size_t index, const std::vector<float>& vertices, bool visible, const CFrame& cframe) {
    if (index < models.size()) {
        models[index].vertices = vertices;
        models[index].visible = visible;
        models[index].cframe = cframe;
    }
}

static LuauExport Luau3dExports[] = {
    {"setClearColor", Luau3D::setClearColor},
    {"getDeltaTime", Luau3D::getDeltaTime},
    {"isRunning", Luau3D::isRunning},
    {"addModel", Luau3D::addModel},
    {"removeModel", Luau3D::removeModel},
    {"clearModels", Luau3D::clearModels},
    {"setModelVisible", Luau3D::setModelVisible},
    {"updateModel", Luau3D::updateModel},
    {"setLight", Luau3D::setLight},
    {"registerBeforeRenderCallback", Luau3D::registerBeforeRenderCallback},
    {nullptr, nullptr}
};

const LuauExport* Luau3D::getExports() const {
    return Luau3dExports;
} 