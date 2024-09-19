cmake_minimum_required(VERSION 3.29)
project(GameEngine)

set(CMAKE_CXX_STANDARD 14)

# For macos, allow float coercions to int. This is the default behaviour in windows.
if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    add_compile_options(-Wno-narrowing)
endif()

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

include_directories(GameEngine/Collision)
include_directories(GameEngine/Core)
include_directories(GameEngine/Entities)
include_directories(GameEngine/Input)
include_directories(GameEngine/Physics)
include_directories(GameEngine/Networking)

add_executable(GameEngine
    Game/main.cpp # For running the actual game
#    GameEngine/main.cpp # For running the engine, with its test file

        Dependencies/include/ZMQ/zmq.hpp
        Dependencies/include/ZMQ/zmq.h
        Dependencies/include/ZMQ/zmq_utils.h
        GameEngine/Core/GameEngine.cpp
    GameEngine/Core/Renderer.cpp
    GameEngine/Core/Window.cpp
        GameEngine/Input/InputManager.h
        GameEngine/Input/InputManager.cpp
        GameEngine/Physics/PhysicsSystem.cpp
        GameEngine/Entities/Entity.cpp
        GameEngine/Collision/CollisionSystem.cpp
        GameEngine/Networking/Client.h
        GameEngine/Networking/Server.h
        GameEngine/Networking/Client.cpp
        GameEngine/Networking/Server.cpp)

target_link_libraries(GameEngine ${SDL2_LIBRARIES})
target_link_libraries(GameEngine zmq)