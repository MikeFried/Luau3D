#include "Luau3D.h"
#include "lua.h"
#include "lualib.h"
#include <iostream>
#include <vector>
#include <cmath>

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
    
    if (instance->renderer->hasGeometryToDraw) {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glTranslatef(0.0f, 0.0f, -1.5f);
        
        static float angle = 0.0f;
        angle += 1.0f;
        glRotatef(angle, 1.0f, 1.0f, 0.0f);
        
        // Enable vertex arrays for both position and color
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        
        const float* data = instance->renderer->geometryVertices.data();
        // Stride is 6 floats (3 for position, 3 for color)
        glVertexPointer(3, GL_FLOAT, 6 * sizeof(float), data);
        // Color data starts 3 floats after each vertex
        glColorPointer(3, GL_FLOAT, 6 * sizeof(float), data + 3);
        
        glDrawArrays(GL_TRIANGLES, 0, instance->renderer->geometryVertices.size() / 6);
        
        glDisableClientState(GL_COLOR_ARRAY);
        glDisableClientState(GL_VERTEX_ARRAY);
        
        instance->renderer->hasGeometryToDraw = false;
    }
    
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
    
    // Store the geometry data for rendering during present
    instance->renderer->setGeometryData(vertices);
    
    return 0;
}

int Luau3D::setLight(lua_State* L) {
    Luau3D* instance = getInstance(L);
    if (!instance) return 0;
    
    // Get light number (1-8) and convert to GL_LIGHT0-GL_LIGHT7
    int lightNum = luaL_checkinteger(L, 1) - 1;
    if (lightNum < 0 || lightNum > 7) {
        luaL_error(L, "Light number must be between 1 and 8");
        return 0;
    }
    GLenum light = GL_LIGHT0 + lightNum;
    
    // Check properties table
    luaL_checktype(L, 2, LUA_TTABLE);
    
    // Helper function to get float array from table field
    auto getFloatArray = [L](const char* field, float* out, int minSize, int maxSize) -> int {
        lua_getfield(L, 2, field);
        if (lua_istable(L, -1)) {
            int size = lua_objlen(L, -1);
            if (size >= minSize && size <= maxSize) {
                for (int i = 0; i < size; i++) {
                    lua_rawgeti(L, -1, i + 1);
                    out[i] = static_cast<float>(lua_tonumber(L, -1));
                    lua_pop(L, 1);
                }
                // Fill remaining with defaults
                for (int i = size; i < maxSize; i++) {
                    out[i] = (i == 3) ? 1.0f : 0.0f; // w = 1.0 for position/colors
                }
                lua_pop(L, 1);
                return size;
            }
        }
        lua_pop(L, 1);
        return 0;
    };
    
    // Helper function to get single float from table field
    auto getFloat = [L](const char* field, float defaultVal) -> float {
        lua_getfield(L, 2, field);
        float result = lua_isnumber(L, -1) ? static_cast<float>(lua_tonumber(L, -1)) : defaultVal;
        lua_pop(L, 1);
        return result;
    };
    
    // Process each possible light property
    float values[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    
    // Position (3 or 4 components)
    if (getFloatArray("position", values, 3, 4)) {
        glLightfv(light, GL_POSITION, values);
    }
    
    // Ambient color (3 or 4 components)
    if (getFloatArray("ambient", values, 3, 4)) {
        glLightfv(light, GL_AMBIENT, values);
    }
    
    // Diffuse color (3 or 4 components)
    if (getFloatArray("diffuse", values, 3, 4)) {
        glLightfv(light, GL_DIFFUSE, values);
    }
    
    // Specular color (3 or 4 components)
    if (getFloatArray("specular", values, 3, 4)) {
        glLightfv(light, GL_SPECULAR, values);
    }
    
    // Spot direction (3 components)
    if (getFloatArray("spotDirection", values, 3, 3)) {
        glLightfv(light, GL_SPOT_DIRECTION, values);
    }
    
    // Spot exponent
    float spotExponent = getFloat("spotExponent", -1.0f);
    if (spotExponent >= 0.0f) {
        glLightf(light, GL_SPOT_EXPONENT, spotExponent);
    }
    
    // Spot cutoff
    float spotCutoff = getFloat("spotCutoff", -1.0f);
    if (spotCutoff >= 0.0f) {
        glLightf(light, GL_SPOT_CUTOFF, spotCutoff);
    }
    
    // Attenuation factors
    float constAtten = getFloat("constantAttenuation", -1.0f);
    if (constAtten >= 0.0f) {
        glLightf(light, GL_CONSTANT_ATTENUATION, constAtten);
    }
    
    float linearAtten = getFloat("linearAttenuation", -1.0f);
    if (linearAtten >= 0.0f) {
        glLightf(light, GL_LINEAR_ATTENUATION, linearAtten);
    }
    
    float quadAtten = getFloat("quadraticAttenuation", -1.0f);
    if (quadAtten >= 0.0f) {
        glLightf(light, GL_QUADRATIC_ATTENUATION, quadAtten);
    }
    
    // Enable the light
    glEnable(light);
    
    lua_pushboolean(L, 1);
    return 1;
}

static LuauExport Luau3dExports[] = {
    {"setClearColor", Luau3D::setClearColor},
    {"getDeltaTime", Luau3D::getDeltaTime},
    {"isRunning", Luau3D::isRunning},
    {"present", Luau3D::present},
    {"drawGeometry", Luau3D::drawGeometry},
    {"setLight", Luau3D::setLight},
    {nullptr, nullptr}
};

const LuauExport* Luau3D::getExports() const {
    return Luau3dExports;
} 