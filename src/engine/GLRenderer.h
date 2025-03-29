#pragma once

#include <string>
#include <GL/gl.h>

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

private:
    int width;
    int height;
    float clearColor[4];
}; 