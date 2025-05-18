#define GL_SILENCE_DEPRECATION
#include "GLRenderer.h"
#import <Cocoa/Cocoa.h>
#include <OpenGL/gl3.h>
#include <iostream>
#include <vector>
#include <cmath>

static const char* vertexShaderSrc = R"(
#version 150
in vec3 aPos;
in vec3 aColor;
out vec3 vColor;
uniform mat4 uMVP;
void main() {
    vColor = aColor;
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

static const char* fragmentShaderSrc = R"(
#version 150
in vec3 vColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(vColor, 1.0);
}
)";

GLRenderer::GLRenderer(IGUI* gui)
    : gui(gui), glContext(nullptr), glView(nullptr), width(800), height(600),
      shaderProgram(0), vao(0), vbo(0), uMVP(-1), glInited(false) {
    clearColor[0] = clearColor[1] = clearColor[2] = 0.0f;
    clearColor[3] = 1.0f;
    std::cout << "[Mac] GLRenderer constructed" << std::endl;
}

GLRenderer::~GLRenderer() {
    destroyGLObjects();
    if (glContext) {
        NSOpenGLContext* ctx = (NSOpenGLContext*)glContext;
        [ctx clearDrawable];
        [ctx release];
        glContext = nullptr;
    }
    if (glView) {
        [(NSView*)glView removeFromSuperview];
        [((NSObject*)glView) release];
        glView = nullptr;
    }
    std::cout << "[Mac] GLRenderer destroyed" << std::endl;
}

void GLRenderer::ensureGLObjects() {
    if (glInited) return;
    setupShaders();
    setupBuffers();
    glInited = true;
}

void GLRenderer::destroyGLObjects() {
    if (vbo) glDeleteBuffers(1, &vbo);
    if (vao) glDeleteVertexArrays(1, &vao);
    if (shaderProgram) glDeleteProgram(shaderProgram);
    vbo = vao = shaderProgram = 0;
    glInited = false;
}

void GLRenderer::setupShaders() {
    auto compile = [](GLenum type, const char* src) -> GLuint {
        GLuint s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        GLint ok = 0; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512]; glGetShaderInfoLog(s, 512, nullptr, log);
            std::cerr << "Shader error: " << log << std::endl;
        }
        return s;
    };
    GLuint vs = compile(GL_VERTEX_SHADER, vertexShaderSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fragmentShaderSrc);
    shaderProgram = glCreateProgram();
    
    // Bind attribute locations BEFORE linking
    glBindAttribLocation(shaderProgram, 0, "aPos");
    glBindAttribLocation(shaderProgram, 1, "aColor");
    
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);
    
    GLint linked = 0;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &linked);
    if (!linked) {
        char log[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, log);
        std::cerr << "Program link error: " << log << std::endl;
    }
    glDeleteShader(vs);
    glDeleteShader(fs);
    
    // Get uniform locations
    uMVP = glGetUniformLocation(shaderProgram, "uMVP");
    if (uMVP == -1) {
        std::cerr << "Warning: Could not find uMVP uniform in shader program" << std::endl;
    }
}

void GLRenderer::setupBuffers() {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
}

void GLRenderer::setMVP(const float* mvp) {
    glUniformMatrix4fv(uMVP, 1, GL_FALSE, mvp);
}

bool GLRenderer::initialize() {
    WindowInfo info = gui->getWindowInfo();
    NSWindow* win = (NSWindow*)info.handle;
    width = info.width;
    height = info.height;
    if (!win) {
        std::cerr << "[Mac] GLRenderer: No window handle" << std::endl;
        return false;
    }
    NSRect frame = NSMakeRect(0, 0, width, height);
    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 24,
        NSOpenGLPFAStencilSize, 8,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        0
    };
    NSOpenGLPixelFormat* pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];
    if (!pixelFormat) {
        std::cerr << "[Mac] GLRenderer: Failed to create pixel format" << std::endl;
        return false;
    }
    NSView* contentView = [[NSView alloc] initWithFrame:frame];
    [win setContentView:contentView];
    NSOpenGLContext* ctx = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:nil];
    [ctx setView:contentView];
    [ctx makeCurrentContext];
    glView = contentView;
    glContext = ctx;
    [contentView release];
    [pixelFormat release];
    ensureGLObjects();
    
    // Enable depth testing just like Windows version does
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Set viewport
    glViewport(0, 0, width, height);
    
    // Print OpenGL version/capabilities
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* vendor = glGetString(GL_VENDOR);
    std::cout << "[Mac] OpenGL Version: " << (version ? (const char*)version : "null") << std::endl;
    std::cout << "[Mac] OpenGL Renderer: " << (renderer ? (const char*)renderer : "null") << std::endl;
    std::cout << "[Mac] OpenGL Vendor: " << (vendor ? (const char*)vendor : "null") << std::endl;
    // Print window and view size
    NSRect winFrame = [win frame];
    NSRect viewFrame = [((NSView*)glView) frame];
    std::cout << "[Mac] Window size: " << winFrame.size.width << "x" << winFrame.size.height << std::endl;
    std::cout << "[Mac] View size: " << viewFrame.size.width << "x" << viewFrame.size.height << std::endl;
    std::cout << "[Mac] GLRenderer initialized" << std::endl;
    return true;
}

void GLRenderer::beginFrame() {
    NSOpenGLContext* ctx = (NSOpenGLContext*)glContext;
    [ctx makeCurrentContext];
}

void GLRenderer::endFrame() {
    NSOpenGLContext* ctx = (NSOpenGLContext*)glContext;
    [ctx flushBuffer];
    [ctx update];
}

void GLRenderer::clear() {
    glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Make sure depth testing is enabled
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
}

void GLRenderer::setClearColor(float r, float g, float b, float a) {
    clearColor[0] = r;
    clearColor[1] = g;
    clearColor[2] = b;
    clearColor[3] = a;
}

void GLRenderer::setLight(int lightNum, const LightProperties& properties) {
    std::cout << "[Mac] Set light " << lightNum << std::endl;
}

void GLRenderer::enableLighting(bool enable) {
    std::cout << "[Mac] " << (enable ? "Enable" : "Disable") << " lighting" << std::endl;
}

void GLRenderer::render(const std::vector<Model>& models) {
    ensureGLObjects();
    glGetError(); // Clear any previous errors
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    glUseProgram(shaderProgram);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    // Set up vertex attributes
    glEnableVertexAttribArray(0); // Position
    glEnableVertexAttribArray(1); // Color
    
    const int stride = 6 * sizeof(float);
    
    // Now try to draw the models
    for (const auto& model : models) {
        if (!model.visible || model.vertices.empty()) continue;
        
        // Upload model data to GPU
        glBufferData(GL_ARRAY_BUFFER, model.vertices.size() * sizeof(float), 
                     model.vertices.data(), GL_DYNAMIC_DRAW);
        
        // Set up vertex attributes for model
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        
        // Use projection matrix
        float projectionMatrix[16] = {
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, -1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        setMVP(projectionMatrix);
        
        // Draw model
        int vertexCount = model.vertices.size() / 6;
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    }
    
    // Clean up
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glBindVertexArray(0);
    glUseProgram(0);
} 