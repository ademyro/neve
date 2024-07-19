cmake_minimum_required(VERSION 3.0.0)
project(neve) # VERSION 0.0.0-20211225

add_executable(neve
  src/main.c
  src/chunk.c
  src/mem.c
  src/debug.c
  src/val.c
)

target_include_directories(neve PRIVATE 
  include/
)

target_compile_options(neve PRIVATE
  -Wall
  -Wextra
  -Wconversion
  -Werror
  -pedantic
  -g
)

add_custom_target(
  clang-tidy-check clang-tidy -p ${CMAKE_BINARY_DIR}/compile_commands.json -checks=cert* ${sources}
  DEPENDS ${sources}
)
