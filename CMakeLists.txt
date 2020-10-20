cmake_minimum_required(VERSION 3.8)

project(bazcal LANGUAGES C CXX)

add_subdirectory(deps/aegis)

add_executable(bazcal
    src/main.cc
    src/embeds.cc
    src/handlers.cc
)

include_directories(include)

target_link_libraries(bazcal aegis pthread)