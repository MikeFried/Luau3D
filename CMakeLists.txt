cmake_minimum_required(VERSION 3.15)
project(Luau3D VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialize and update Luau submodule if not present
if(NOT EXISTS "${PROJECT_SOURCE_DIR}/external/luau/CMakeLists.txt")
    message(STATUS "Initializing Luau submodule...")
    execute_process(
        COMMAND git submodule update --init --recursive external/luau
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
        RESULT_VARIABLE GIT_RESULT
    )
    if(NOT GIT_RESULT EQUAL 0)
        message(FATAL_ERROR "Failed to initialize Luau submodule")
    endif()
endif()

# Platform-specific source files
set(PLATFORM_SOURCES)
if(WIN32)
    set(PLATFORM_SOURCES
        src/engine/Windows/GLRenderer.cpp
        src/engine/Windows/GLRenderer.h
        src/engine/Windows/GUI.cpp
        src/engine/Windows/GUI.h
    )
elseif(APPLE)
    set(PLATFORM_SOURCES
        src/engine/Mac/GLRenderer.cpp
        src/engine/Mac/GLRenderer.h
        src/engine/Mac/GUI.cpp
        src/engine/Mac/GUI.h
    )
endif()

# Add source files
set(SOURCES
    src/main.cpp
    src/engine/Engine.cpp
    src/engine/Engine.h
    src/engine/Luau3D.cpp
    src/engine/Luau3D.h
    src/engine/LuauBinding.cpp
    src/engine/LuauBinding.h
    src/engine/Config.cpp
    src/engine/Config.h
    ${PLATFORM_SOURCES}
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Copy the scripts folder to the output directory
file(COPY ${PROJECT_SOURCE_DIR}/scripts DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

# Add header directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/src
    ${PROJECT_SOURCE_DIR}/external/luau/Common/include
    ${PROJECT_SOURCE_DIR}/external/luau/VM/include
    ${PROJECT_SOURCE_DIR}/external/luau/Compiler/include
    ${PROJECT_SOURCE_DIR}/external/luau/Ast/include
)

# Add Luau subdirectory
add_subdirectory(external/luau)

# Link against Luau libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Luau.Ast
    Luau.Compiler
    Luau.VM
    Luau.Common
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE WIN32_LEAN_AND_MEAN)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32)
elseif(APPLE)
    find_library(COCOA_LIBRARY Cocoa)
    find_library(OPENGL_LIBRARY OpenGL)
    find_library(IOKIT_LIBRARY IOKit)
    find_library(COREVIDEO_LIBRARY CoreVideo)
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${COCOA_LIBRARY}
        ${OPENGL_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
    
    # Set minimum macOS version
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_RPATH 1
        MACOSX_DEPLOYMENT_TARGET "10.15"
    )
endif() 