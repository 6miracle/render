cmake_minimum_required(VERSION 3.15)
project(Render LANGUAGES C CXX)

# 设置c++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)
# set(BUILD_TEST ON)

#设置输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME})

set(CMAKE_BUILD_TYPE RELEASE)


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp src/*.h src/*.hpp )
add_executable(${PROJECT_NAME} ${SOURCES})

include_directories(vendor)
include_directories(vendor/include)
link_directories(vendor/lib)

target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.h)
target_include_directories(${PROJECT_NAME} PUBLIC  src )
# target_link_directories(${PROJECT_NAME} PUBLIC vendor/lib)
target_link_libraries(${PROJECT_NAME} PUBLIC opengl32  H:/code/render/vendor/lib/glfw3.dll) 