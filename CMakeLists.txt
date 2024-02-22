cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project(DuoDyno LANGUAGES CXX)

option(BOX2D_BUILD_SAMPLES "Build the box2d-lite sample program" OFF)

include_directories(include)
include_directories(include/view/common)

if (BOX2D_BUILD_SAMPLES)
	add_subdirectory(extern/glfw)
	add_subdirectory(extern/glad)
	add_subdirectory(extern/metal-cpp)
	add_subdirectory(metal-cpp-cmake)
	# default startup project for Visual Studio

endif()


add_executable(sample main.cpp
		src/view/common/shape.cpp
		src/view/common/shape_factory.cpp
		src/view/window.cpp
		src/view/renderer.cpp
)
target_link_libraries(sample PUBLIC glfw glad METAL_CPP ${OPENGL_LIBRARIES})
target_include_directories(sample PRIVATE include)