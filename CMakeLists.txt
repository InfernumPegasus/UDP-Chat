cmake_minimum_required(VERSION 3.20)
project(Lab11)

set(CMAKE_C_STANDARD 23)

add_executable(server server.c)
add_executable(client client.c)
