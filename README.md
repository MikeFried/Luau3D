# Luau3D Game Engine

A simple cross-platform game engine prototype using C++, Luau VM, and OpenGL.

## Features

- C++ core engine
- Luau scripting support
- OpenGL rendering
- Cross-platform support (currently Windows, more platforms coming later)
- No external dependencies required

### Working features
- Launch with parameters, load a Luau file
- Luau require works relative to the Luau file
- Overrides for Require to load binary modules
- 2 binary modules exist: Luau3D and GUI
- Renders 3D meshes from Luau
- Keyboard input integrated into GUI module
- Accurate to the millisecond timing for beforeUpdate math

### Technical debt / planned changes
- C++ binding mechanism needs a way to dynamically load DLL as runtime
- Will likely rework ILuauModule
- Need to clean up message pump
- More control over GUI window needs to be given to Luau
- Need to remove tight global dependencies between GLRenderer and GUI module as they flow the wrong way

### Bugs
- Window event processing pauses frames when dragging

### Future planned features - in no particular order

#### Game functionality
- Document Model / Tree and JSON serialization
- Scene graph adapter / view on Document Model
- Filesystem operations
- Network support for running as server and client and DM replication
- Network support for ICE+ / STUN / TURN and P2P
#### 3D Rendering
- Camera
- Loading / Saving Meshes and Textures
- NURBS / Parametric Meshes
- Pixel / Texture / Geometry Shader support
#### Inputs
- Keyboard
- Mouse
- Stream Deck
- Game pads
#### Audio Support
- Audio Source Selection
- Audio Inputs and Outputs
- MIDI Source Selection
- MIDI Inputs and Outputs
#### Streaming Support
- Video Device Selection
- Video Input / Outputs
#### Filters
- GPU Kernel filters for Audio / Video
#### Debugging
- Integrate DAP from Open Source project and make template for VS Code starter project including DAP
#### Porting
- Linux
- Mac OS
- iPad
- Android

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

- `src/`- Source files
 - `engine/`- Core engine components
 - `main.cpp`- Entry point
- `scripts/`- Luau game scripts
- `external/`- Third-party dependencies

## Usage

The engine uses Luau scripts for game logic. The current script is located at `scripts/test.lua` and copied into the running folder for now.
Command line arguments include --run filename to specify the game starting script.
If no command line arguments are passed, the executable tries to run `main.luau` in the current working folder.

