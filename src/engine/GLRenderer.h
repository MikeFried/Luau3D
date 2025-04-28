#pragma once

#include <string>
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <vector>

struct Model {
    std::vector<float> vertices;  // Interleaved position (3) and color (3) data
    bool visible;
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

class GLRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    // Initialize OpenGL context and window
    bool initialize(const std::string& windowTitle, int width, int height);

    // Begin a new frame
    void beginFrame();

    // End the current frame
    void endFrame();

    // Clear the screen
    void clear();

    // Set clear color
    void setClearColor(float r, float g, float b, float a);

    // Get window dimensions
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    // Check if window is still open
    bool isWindowOpen() const;

    // Light management
    void setLight(int lightNum, const LightProperties& properties);
    void enableLighting(bool enable);

    // Render all visible models
    void render(const std::vector<Model>& models);

private:
    int width;
    int height;
    float clearColor[4];
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    bool windowOpen;
}; 