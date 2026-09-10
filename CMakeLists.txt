cmake_minimum_required(VERSION 3.20)
project(rps-board-game CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# IMPORTANT: use the ucrt64 terminal to generate build files, not the 
#   standard windows terminal - this enforces the compiler to gcc instead of msvc
#
# - run `cmake -B {{ build_folder_name }}` to generate build files 
#   (only update if this file changes)
#
# - run `cmake --build {{ build_folder_name }}` to generate artifacts from build files
# ------------------------------------------------------------------------------

# ------------------------------------------------------------------------------
# Output Directory Configuration
# ------------------------------------------------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/debug")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/release")

# ------------------------------------------------------------------------------
# Sources & Executable
# ------------------------------------------------------------------------------
file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${SOURCES})

# ------------------------------------------------------------------------------
# Statically link MinGW runtime dependencies (gcc, stdc++, winpthread)
# ------------------------------------------------------------------------------
if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE 
        -static 
        -static-libgcc 
        -static-libstdc++
    )
endif()

# ------------------------------------------------------------------------------
# SDL3 Configuration
# ------------------------------------------------------------------------------
# Set this path to your SDL3 development directory if CMake doesn't find it automatically
# set(SDL3_DIR "path/to/SDL3/lib/cmake/SDL3")

find_package(SDL3 REQUIRED CONFIG)
find_package(SDL3_ttf REQUIRED CONFIG)

# Link against the shared SDL3 library target
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3-shared)
target_link_libraries(${PROJECT_NAME} PRIVATE SDL3_ttf::SDL3_ttf-shared)

# ------------------------------------------------------------------------------
# Copy SDL3.dll to the Output Directory (Windows)
# ------------------------------------------------------------------------------
if(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SDL3::SDL3-shared>
            $<TARGET_FILE:SDL3_ttf::SDL3_ttf-shared>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        VERBATIM
    )
endif()