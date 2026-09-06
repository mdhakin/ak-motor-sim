cmake_minimum_required(VERSION 3.20)

project(ak_motor_sim
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(external/ftxui)

add_executable(ak-motor-sim
    src/main.cpp
    src/can/socketcan.cpp
    src/protocol/cubemars_mit.cpp
)

target_link_libraries(ak-motor-sim
    PRIVATE
        ftxui::screen
        ftxui::dom
        ftxui::component
)

target_compile_options(ak-motor-sim
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
)