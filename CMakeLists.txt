cmake_minimum_required(VERSION 3.0.0)
project(rockcore VERSION 0.1.0)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CTest)
enable_testing()

# glfw3
find_package(glfw3 REQUIRED)
include_directories(${glfw3_INCLUDE_DIR})

# glad
include_directories(./3rdparty/glad/include/)
add_subdirectory(./3rdparty/glad)

# glm
include_directories(./3rdparty/glm-0.9.9.8/)
add_subdirectory(./3rdparty/glm-0.9.9.8/)

# stb_image
include_directories(./3rdparty/stb_image/)

add_executable(${PROJECT_NAME} opengl_helper.cpp main.cpp)

target_link_libraries(${PROJECT_NAME} glfw glad glm)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
