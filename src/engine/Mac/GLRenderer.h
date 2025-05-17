#pragma once

#include "../IRenderer.h"
#include "../IGUI.h"
#include <iostream>

class GLRenderer : public IRenderer {
public:
    GLRenderer(IGUI* gui);
    ~GLRenderer();

    // Initialize OpenGL context
    bool initialize() override;

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

    // Light management
    void setLight(int lightNum, const LightProperties& properties) override;
    void enableLighting(bool enable) override;

    // Render all visible models
    void render(const std::vector<Model>& models) override;

private:
    float clearColor[4];
    IGUI* gui;
    int width;
    int height;
    void* glContext; // (NSOpenGLContext*)
    void* glView;    // (NSOpenGLView*)

    // Modern OpenGL
    unsigned int shaderProgram;
    unsigned int vao, vbo;
    int uMVP; // uniform location for MVP matrix
    bool glInited;
    void ensureGLObjects();
    void destroyGLObjects();
    void setupShaders();
    void setupBuffers();
    void setMVP(const float* mvp);
}; 