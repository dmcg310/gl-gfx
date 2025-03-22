# gl-gfx

A (somewhat) batched, instanced renderer written in OpenGL. Includes ImGui for entity controls, scene loading/saving. A camera system, scene serialisation/deserialisation with TOML, directional and point lighting, and mesh loading with Assimp.

![image](https://github.com/user-attachments/assets/d568f0d8-2d2e-4cd9-b331-57321c13e8b5)

## Features

- Batched instanced rendering for efficient drawing
- Entity-component style scene management
- Material system with PBR-like properties
- Point and directional lighting
- Camera system with mouse/keyboard navigation
- Scene serialization with TOML
- ImGui integration for interactive editing

## Building the Project

The project uses CMake with presets for easier build configuration.

### Prerequisites

CMake 3.19 or newer
C++20 compatible compiler
Git

### Setup
Clone the repository and initialize submodules:

```sh
git clone https://github.com/yourusername/gl-gfx.git
cd gl-gfx
git submodule update --init --recursive
```

### Building with CMake Presets
Debug Build (with UI debug features)

#### Configure debug build
```sh
cmake --preset=debug
```

#### Build with parallel jobs
cmake --build --preset=debug -j
Release Build (without debug UI)
bashCopy# Configure release build
cmake --preset=release

# Build with parallel jobs
cmake --build --preset=release -j

Running the Application
After building, run the application:
bashCopy# Debug build
./build-debug/gl-gfx/gl-gfx

# Release build
./build-release/gl-gfx/gl-gfx
Debug vs Release Modes

Debug Mode: Includes ImGui panels for entity manipulation, scene editing, and performance metrics
Release Mode: UI debug panels are disabled for better performance in deployed applications

Toggle between camera and UI modes with Tab key.

### References

- [Glitter](https://github.com/Polytonic/Glitter)
- [Assimp](https://github.com/assimp/assimp)
- [tomlplusplus](https://github.com/marzer/tomlplusplus)
- [ImGui](https://github.com/ocornut/imgui)
- [GLM](https://github.com/g-truc/glm)
- [Default Texture](https://polyhaven.com/a/rebar_reinforced_concrete)
