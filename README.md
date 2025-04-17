# Luau3D Game Engine

A simple cross-platform game engine prototype using C++, Luau VM, and OpenGL.

## Features

- C++ core engine
- Luau scripting support
- OpenGL rendering
- Cross-platform support (currently Windows, more platforms coming later)
- No external dependencies required

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Git (for cloning the repository)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/MikeFried/Luau3D.git
cd Luau3D
```

2. Create a build directory and generate build files:
```bash
mkdir build
cd build
cmake ..
```

3. Build the project:
```bash
cmake --build .
```

## Project Structure

- `src/` - Source files
  - `engine/` - Core engine components
  - `main.cpp` - Entry point
- `scripts/` - Luau game scripts
- `external/` - Third-party dependencies

## Usage

The engine uses Luau scripts for game logic. The current script is located at `scripts/test.lua` and copied into the running folder for now.
Command line arguments include --run filename to specify the game starting script.
If no command line arguments are passed, the executable tries to run `main.luau` in the current working folder.

