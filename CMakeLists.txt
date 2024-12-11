cmake_minimum_required(VERSION 3.0)
project(final_project)

find_package(OpenGL REQUIRED)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

add_subdirectory(external)

include_directories(
    external/glfw-3.1.2/include/
    external/glm-0.9.7.1/
    external/glad-opengl-3.3/include/
    external/tinygltf-2.9.3/
    final_project/
)

add_executable(final_project
    final_project/final_project.cpp
    final_project/render/shader.cpp
    final_project/camera/camera.cpp
    final_project/Objects/Cube/Cube.cpp
    final_project/Objects/skybox/SkyBox.cpp
    final_project/texture_utils/texture_utils.cpp
    final_project/objects/gltf_object/GltfObject.cpp final_project/lighting/light/Light.cpp
    final_project/tinygltf_implementation.cpp
    final_project/objects/graphics_object/GraphicsObject.h final_project/objects/graphics_object/GraphicsObject.cpp final_project/lighting/lights_manager/LightsManager.cpp final_project/lighting/lights_manager/LightsManager.h
)

target_link_libraries(final_project
    ${OPENGL_LIBRARY}
    glfw
    glad
)

