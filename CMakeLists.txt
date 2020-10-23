cmake_minimum_required(VERSION 3.8)

project(bazcal-lf LANGUAGES C CXX)

add_subdirectory(deps/aegis)

add_subdirectory(deps/bazcal)

add_executable(bazcal-lf
    src/main.cc
    src/embeds.cc
    src/handlers.cc
    src/utils.cc
)

include_directories(include)
include_directories(deps/bazcal/include)

find_package(Threads)

target_link_libraries(bazcal-lf aegis Threads::Threads bazcal curl sqlite3)