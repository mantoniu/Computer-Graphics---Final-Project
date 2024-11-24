cmake_minimum_required(VERSION 3.0)
project(final_project)

# Trouver OpenGL
find_package(OpenGL REQUIRED)

# Définir les répertoires de sortie pour les bibliothèques, les exécutables et les archives
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

# Ajouter le sous-répertoire pour les dépendances externes
add_subdirectory(external)

# Définir les répertoires d'inclusion
include_directories(
    external/glfw-3.1.2/include/
    external/glm-0.9.7.1/
    external/glad-opengl-3.3/include/
    external/
    final_project/
)

# Ajouter un nouvel exécutable pour lab2_skybox
add_executable(final_project
    final_project/final_project.cpp
    final_project/render/shader.cpp
)

target_link_libraries(final_project
    ${OPENGL_LIBRARY}
    glfw
    glad
)

