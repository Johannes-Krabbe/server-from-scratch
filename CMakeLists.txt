cmake_minimum_required(VERSION 3.16.3)
project(server-from-scratch C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS "-Wall -Werror -Wextra -Wno-unused-parameter")

add_executable(server main.c)
