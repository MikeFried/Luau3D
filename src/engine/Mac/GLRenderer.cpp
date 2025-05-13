#include "GLRenderer.h"

GLRenderer::GLRenderer(IGUI* gui) : gui(gui), width(800), height(600) {
    clearColor[0] = clearColor[1] = clearColor[2] = 0.0f;
    clearColor[3] = 1.0f;
    std::cout << "[Mac] GLRenderer constructed" << std::endl;
}

GLRenderer::~GLRenderer() {
    std::cout << "[Mac] GLRenderer destroyed" << std::endl;
}

bool GLRenderer::initialize() {
    std::cout << "[Mac] GLRenderer initialized" << std::endl;
    return true;
}

void GLRenderer::beginFrame() {
    std::cout << "[Mac] Begin frame" << std::endl;
}

void GLRenderer::endFrame() {
    std::cout << "[Mac] End frame" << std::endl;
}

void GLRenderer::clear() {
    std::cout << "[Mac] Clear screen with color: " 
              << clearColor[0] << ", " 
              << clearColor[1] << ", " 
              << clearColor[2] << ", " 
              << clearColor[3] << std::endl;
}

void GLRenderer::setClearColor(float r, float g, float b, float a) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
    std::cout << "[Mac] Set clear color: " << r << ", " << g << ", " << b << ", " << a << std::endl;
}

void GLRenderer::setLight(int lightNum, const LightProperties& properties) {
    std::cout << "[Mac] Set light " << lightNum << std::endl;
}

void GLRenderer::enableLighting(bool enable) {
    std::cout << "[Mac] " << (enable ? "Enable" : "Disable") << " lighting" << std::endl;
}

void GLRenderer::render(const std::vector<Model>& models) {
    std::cout << "[Mac] Rendering " << models.size() << " models" << std::endl;
    for (const auto& model : models) {
        if (model.visible) {
            std::cout << "  - Model with " << model.vertices.size() << " vertices" << std::endl;
        }
    }
} 