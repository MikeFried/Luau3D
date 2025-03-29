cmake_minimum_required(VERSION 3.15)
project(Luau3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files
set(SOURCES
    src/main.cpp
    src/engine/Engine.cpp
    src/engine/Engine.h
    src/engine/LuauBinding.cpp
    src/engine/LuauBinding.h
    src/engine/GLRenderer.cpp
    src/engine/GLRenderer.h
)

# Add header directories
include_directories(
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/vendor/luau
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Add Luau as a subdirectory (we'll need to add the Luau source files)
add_subdirectory(vendor/luau)

# Link against Luau
target_link_libraries(${PROJECT_NAME} PRIVATE Luau.Ast Luau.VM Luau.Compiler)

# OpenGL
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE GL)
endif() 