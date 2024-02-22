project(glad C)
add_library(glad STATIC src/gles2.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/include)