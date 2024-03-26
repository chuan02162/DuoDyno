cmake_minimum_required(VERSION 3.8)
set(CMAKE_CXX_STANDARD 20)

project(DuoDyno LANGUAGES CXX)

option(SS ON)
set(SS FORCE OFF)

include_directories(include)
include_directories(include/view/common)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glad)
add_subdirectory(extern/metal-cpp)
add_subdirectory(metal-cpp-cmake)

add_executable(sample main.cpp
		src/view/common/shape.cpp
		src/view/common/shape_factory.cpp
		src/view/window.cpp
		src/view/task_queue.cpp
		src/view/common/controller.cpp
		include/engine/physics/world.h
		include/engine/physics/body.h
		src/engine/physics/world.cpp
		include/engine/physics/arbiter.h
		src/engine/physics/body.cpp
		include/engine/common/engine_settings.h
		include/engine/physics/collider.h
		src/engine/common/engine_settings.cpp
		src/engine/physics/arbiter.cpp
		src/engine/physics/collide.cpp
		include/engine/common/math_util.h
		src/engine/physics/collider.cpp
)
target_link_libraries(sample PUBLIC glfw glad METAL_CPP ${OPENGL_LIBRARIES})
target_include_directories(sample PRIVATE include)