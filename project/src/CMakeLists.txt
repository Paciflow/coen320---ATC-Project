cmake_minimum_required(VERSION 3.26)
project(ATC)

set(CMAKE_CXX_STANDARD 17)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Find GLFW
find_package(GLFW3 REQUIRED)

# Manually specify GLEW paths
set(GLEW_INCLUDE_DIR "/usr/local/opt/glew/include")
set(GLEW_LIBRARY "/usr/local/opt/glew/lib/libGLEW.dylib")

include_directories(${OPENGL_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${GLEW_INCLUDE_DIR} .) # Added current directory for header files

# Find GLM
set(glm_DIR "/usr/local/opt/glm/lib/cmake/glm")  # Add the path where glmConfig.cmake is located
find_package(glm REQUIRED)

# Add executable with all source files
add_executable(ATC main.cpp Airspace.cpp Aircraft.cpp)

# Link libraries
target_link_libraries(ATC ${OPENGL_LIBRARIES} ${GLFW_LIBRARIES} ${GLEW_LIBRARY} ${glm_LIBRARIES})
