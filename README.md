# Luau3D Game Engine

A simple cross-platform game engine prototype using C++, Luau VM, and OpenGL.

## Features

- C++ core engine
- Luau scripting support
- OpenGL rendering
- Cross-platform support (currently Windows, more platforms coming soon)
- No external dependencies required

## Building

### Prerequisites

- CMake 3.15 or higher
- C++17 compatible compiler
- Git (for cloning the repository)

### Build Instructions

1. Clone the repository:
```bash
git clone https://github.com/yourusername/Luau3D.git
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
- `vendor/` - Third-party dependencies

## Usage

The engine uses Luau scripts for game logic. The main script is located at `scripts/main.luau`. You can modify this script to create your game.

## License

This project is licensed under the MIT License - see the LICENSE file for details. 