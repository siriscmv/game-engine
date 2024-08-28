cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 14)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(GameEngine/Collision)
include_directories(GameEngine/Core)
include_directories(GameEngine/Entities)
include_directories(GameEngine/Input)
include_directories(GameEngine/Physics)

add_executable(GameEngine
    Game/main.cpp
    GameEngine/main.cpp
    GameEngine/Core/GameEngine.cpp
    GameEngine/Core/Renderer.cpp
    GameEngine/Core/Window.cpp)

target_link_libraries(GameEngine ${SDL2_LIBRARIES})