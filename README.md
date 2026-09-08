# Developer Environment Setup

- msys2/ucrt64 is installed in the default directory (C:/msys64)
- cmake is installed in the ucrt64 environment (`pacman -Ss cmake --> pacman -S mingw-w64-ucrt-x86_64-cmake`)
- SDL3 is installed in the ucrt64 environment (`pacman -Ss sdl3 --> pacman -S mingw-w64-ucrt-x86_64-sdl3`)

## Generating Builds & Artifacts

- IMPORTANT: use the ucrt64 terminal to generate build files, not the standard windows terminal - this enforces the compiler to gcc instead of msvc
- run `cmake -B {{ build_folder_name }}` to generate build files (only update if this file changes)
- run `cmake --build {{ build_folder_name }}` to generate artifacts from build files
- note: it is OK to use the default terminal to build from build files once they are available
  - however, this means having a duplicate copy of `cmake` in both the ucrt64 environment and the native windows environment