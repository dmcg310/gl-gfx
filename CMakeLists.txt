cmake_minimum_required(VERSION 3.12)
project(gl-gfx)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build (Debug/Release)" FORCE)
endif ()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DGL_GFX_DEBUG_FEATURES_ENABLED)
    message(STATUS "Debug features enabled (Debug build)")
else()
    message(STATUS "Debug features disabled (Release build)")
endif()

option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)
add_subdirectory(src/Vendor/glfw)

option(ASSIMP_BUILD_ASSIMP_TOOLS OFF)
option(ASSIMP_BUILD_SAMPLES OFF)
option(ASSIMP_BUILD_TESTS OFF)
add_subdirectory(src/Vendor/assimp)

if (MSVC)
    add_compile_options(/MP)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /permissive- /std:c++20")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /Od /Zi /RTC1 /MDd")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2 /Oi /Ot /GL /MD")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /LTCG")
    set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /LTCG")
else ()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++20")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -march=native -DNDEBUG")

    if (NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif ()
endif ()

set(IMGUI_SOURCES
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/backends/imgui_impl_opengl3.cpp
)

include_directories(src/Headers/
        src/Vendor/assimp/include/
        src/Vendor/glad/include/
        src/Vendor/glfw/include/
        src/Vendor/glm/
        src/Vendor/stb/
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/
        ${CMAKE_SOURCE_DIR}/src/Vendor/imgui/backends/
        src/Vendor/tomlplusplus/include/)

file(GLOB VENDORS_SOURCES src/Vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS src/Headers/*.hpp)
file(GLOB PROJECT_SOURCES src/Sources/*.cpp)
file(GLOB PROJECT_SHADERS src/Shaders/*.comp
        src/Shaders/*.frag
        src/Shaders/*.geom
        src/Shaders/*.vert)
file(GLOB PROJECT_CONFIGS CMakeLists.txt
        README.md
        .gitignore
        .gitmodules)

source_group("Headers" FILES ${PROJECT_HEADERS})
source_group("Shaders" FILES ${PROJECT_SHADERS})
source_group("Sources" FILES ${PROJECT_SOURCES})
source_group("Vendors" FILES ${VENDORS_SOURCES})
source_group("ImGui" FILES ${IMGUI_SOURCES})
source_group("Configs" FILES ${PROJECT_CONFIGS})

add_definitions(-DGLFW_INCLUDE_NONE -DPROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

add_executable(${PROJECT_NAME}
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_SHADERS}
        ${PROJECT_CONFIGS}
        ${VENDORS_SOURCES}
        ${IMGUI_SOURCES}
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        assimp
        glfw
        ${GLFW_LIBRARIES}
        ${GLAD_LIBRARIES}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/${PROJECT_NAME}
)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/Shaders
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
        DEPENDS ${PROJECT_SHADERS}
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
