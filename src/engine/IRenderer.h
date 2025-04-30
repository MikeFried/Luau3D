#pragma once

#include <string>
#include <vector>

struct CFrame {
    float position[3];    // Position (x, y, z)
    float look[3];        // Forward/look vector
    float up[3];          // Up vector
    float right[3];       // Right vector
    
    CFrame() {
        // Initialize to identity transform
        position[0] = position[1] = position[2] = 0.0f;
        look[0] = 0.0f; look[1] = 0.0f; look[2] = -1.0f;  // Looking down -Z
        up[0] = 0.0f; up[1] = 1.0f; up[2] = 0.0f;         // Up is +Y
        right[0] = 1.0f; right[1] = 0.0f; right[2] = 0.0f; // Right is +X
    }
};

struct Model {
    std::vector<float> vertices;  // Interleaved position (3) and color (3) data
    bool visible;
    CFrame cframe;
    // Future: Add model view information here
    // - Transform matrix
    // - Material properties
    // - Shader information
};

struct LightProperties {
    std::vector<float> position;  // 3 or 4 components
    std::vector<float> ambient;   // 3 or 4 components
    std::vector<float> diffuse;   // 3 or 4 components
    std::vector<float> specular;  // 3 or 4 components
    std::vector<float> spotDirection;  // 3 components
    float spotExponent;
    float spotCutoff;
    float constantAttenuation;
    float linearAttenuation;
    float quadraticAttenuation;
};

class IRenderer {
public:
    virtual ~IRenderer() = default;

    // Initialize renderer context and window
    virtual bool initialize(const std::string& windowTitle, int width, int height) = 0;

    // Begin a new frame
    virtual void beginFrame() = 0;

    // End the current frame
    virtual void endFrame() = 0;

    // Clear the screen
    virtual void clear() = 0;

    // Set clear color
    virtual void setClearColor(float r, float g, float b, float a) = 0;

    // Get window dimensions
    virtual int getWidth() const = 0;
    virtual int getHeight() const = 0;

    // Check if window is still open
    virtual bool isWindowOpen() const = 0;

    // Light management
    virtual void setLight(int lightNum, const LightProperties& properties) = 0;
    virtual void enableLighting(bool enable) = 0;

    // Render all visible models
    virtual void render(const std::vector<Model>& models) = 0;
}; 