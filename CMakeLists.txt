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
include_directories(GameEngine/TimeSystem)
include_directories(GameEngine/Events)
include_directories(GameEngine/Replay)

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
        GameEngine/TimeSystem/Timeline.cpp
        GameEngine/Events/EventManager.cpp
        GameEngine/Events/TypedEventHandler.cpp
        GameEngine/Events/InputEvent.cpp
        GameEngine/Replay/Replay.cpp
)

add_library(GameEngineLib STATIC ${GAME_ENGINE_SOURCES})

add_executable(Client Game/main.cpp) # For running the actual game
add_executable(Server GameEngine/RunServer.cpp) # For running the engine, with its test file

target_link_libraries(Server ${SDL2_LIBRARIES})
target_link_libraries(Server zmq)
target_link_libraries(Server GameEngineLib)

target_link_libraries(Client ${SDL2_LIBRARIES})
target_link_libraries(Client zmq)
target_link_libraries(Client GameEngineLib)
