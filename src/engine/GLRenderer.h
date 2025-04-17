#pragma once

#include <string>
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include <vector>

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

    bool hasGeometryToDraw;
    std::vector<float> geometryVertices;  // Store vertices for rendering

    void setGeometryData(const std::vector<float>& vertices) {
        geometryVertices = vertices;
        hasGeometryToDraw = true;
    }

    // Add these new methods
    void setGeometryColor(float r, float g, float b);
    void enableLighting(bool enable);

private:
    int width;
    int height;
    float clearColor[4];
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    bool windowOpen;
    float geometryColor[3];
}; 