cmake_minimum_required(VERSION 3.23)
project(test_gravite C)

set(CMAKE_C_STANDARD 99)

add_executable(test_gravite main.c)
target_link_libraries(test_gravite -lmingw32 -lallegro-5.0.10-monolith-md-debug)