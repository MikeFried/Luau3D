#pragma once

#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#include "IRenderer.h"

class GLRenderer : public IRenderer {
public:
    GLRenderer();
    ~GLRenderer();

    // Initialize OpenGL context and window
    bool initialize(const std::string& windowTitle, int width, int height) override;

    // Begin a new frame
    void beginFrame() override;

    // End the current frame
    void endFrame() override;

    // Clear the screen
    void clear() override;

    // Set clear color
    void setClearColor(float r, float g, float b, float a) override;

    // Get window dimensions
    int getWidth() const override { return width; }
    int getHeight() const override { return height; }

    // Check if window is still open
    bool isWindowOpen() const override;

    // Light management
    void setLight(int lightNum, const LightProperties& properties) override;
    void enableLighting(bool enable) override;

    // Render all visible models
    void render(const std::vector<Model>& models) override;

private:
    int width;
    int height;
    float clearColor[4];
    HWND hwnd;
    HDC hdc;
    HGLRC hrc;
    bool windowOpen;
}; 