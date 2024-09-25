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

set(GAME_ENGINE_SOURCES
        GameEngine/Core/GameEngine.cpp
        GameEngine/Core/Renderer.cpp
        GameEngine/Core/Window.cpp
        GameEngine/Input/InputManager.cpp
        GameEngine/Physics/PhysicsSystem.cpp
        GameEngine/Entities/Entity.cpp
        GameEngine/Collision/CollisionSystem.cpp
        GameEngine/Networking/Client.cpp
        GameEngine/Networking/Server.cpp
        GameEngine/Networking/Peer.cpp
        GameEngine/Networking/PeerServer.cpp
)

add_library(GameEngineLib STATIC ${GAME_ENGINE_SOURCES})

add_executable(Game Game/main.cpp) # For running the actual game
add_executable(GameEngine GameEngine/main.cpp) # For running the engine, with its test file

target_link_libraries(GameEngine ${SDL2_LIBRARIES})
target_link_libraries(GameEngine zmq)
target_link_libraries(GameEngine GameEngineLib)

target_link_libraries(Game ${SDL2_LIBRARIES})
target_link_libraries(Game zmq)
target_link_libraries(Game GameEngineLib)
