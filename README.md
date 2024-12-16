# GameEngine

- This is a game engine developed using SDL2 in C++ for the course CSC581 - Game Engine Foundations at NC State in the Fall of 2024.
- This project was developed in a team of 3: [siriscmv](https://github.com/siriscmv), [Arcane94](https://github.com/Arcane94), and [djkool757](https://github.com/djkool757).
- This repository is an import of the [original repository](https://github.ncsu.edu/yrajapa/GameEngine) from NCSU's GitHub Enterprise.
- Only the source for the game engine is included in this repository. Sample code for a singleplayer game (Snake) is included below in this file.

## Running the Game

<details>
  <summary>Windows</summary>

## Requirements

1. Visual Studio Community 2022
2. SDL2 (included)
3. ZeroMQ and cppzmq (included)

## How to Run

### Group submission: Compile and run the game engine

1. Open the solution (`GameEngine.sln`) in Visual Studio Community 2022.
2. In the run configurations dropdown:
   - Select **'RunGameEngine'**.
3. Click the 'Local Windows Debugger' button to run the selected configuration within Visual Studio.

### Individual submission step 1: Compile servers, clients, and peers

1. Open the solution (`GameEngine.sln`) in Visual Studio Community 2022.
2. In the run configurations dropdown:
   - Select **'RunClientGame'**.
   - Right-click on the solution in solution explorer and click on 'Rebuid'.
   - Select **'RunPeerGame'**.
   - Right-click on the solution in solution explorer and click on 'Rebuid'.
   - Select **'RunPeerServer'**.
   - Right-click on the solution in solution explorer and click on 'Rebuid'.
   - Select **'RunServer'**.
   - Right-click on the solution in solution explorer and click on 'Rebuid'.

### Individual submission step 2: Run servers, clients and peers

1. To run **'Server-Client'**:
   - Navigate to  `~/x64/RunServer` directory.
   - Double-click **Server.exe** to run the server.
   - Navigate to  `~/x64/RunClientGame` directory.
   - Double-click **ClientGame.exe** to run a client.
   - You can run multiple clients by double-clicking the .exe file again.
2. To run **'Peer-to-Peer'**:
   - Navigate to `~/x64/RunPeerServer`.
   - Double-click the **PeerServer.exe** file to run the peer server.
   - Navigate to `~/x64/RunPeerGame`.
   - Double-click the **PeerGame.exe** file to run a peer.
   - You can run multiple clients by double-clicking the .exe file again.

**Note:**  

- Ensure that the `SDL2.dll` and `libzmq-v143-mt-gd-4_3_5.dll` files are present in the directories where the `.exe` files are located. These file are already included; do not remove it.
- Ensure that you run the servers before clients (or peers).

</details>

<details>
  <summary>MacOS (Silicon)</summary>

## Requirements

- Install SDL2: `brew install sdl2`
- Install CMake: `brew install cmake`
- Install other deps - `brew install cppzmq zeromq nlohmann-json`

## Building and running

- Modify the `CMakeLists.txt` file to include/remove source files as needed
- To compile and run the game: `cmake -S . -B build && cmake --build build`
- The `build` directory will now contain the binaries
- Run the server executable first, then run 1 or more client executables

</details>

## Game Engine Features

- **Cross-platform**: The game engine is built using SDL2 and ZeroMQ and uses CMake for cross-platform compilation.
- **Physics Engine**: The game engine has a simple physics engine that supports collision detection and movement using 2-Dimensional acceleration and velocity vectors.
- **Networking**: The game engine supports both client-server and peer-to-peer networking models using ZeroMQ. JSON is used for serialization and deserialization of game entities and components.
- **Entity Component System**: The game engine uses an Entity Component System (ECS) for managing game entities and their components.
- **Event System**: The game engine is event-driven and supports event handling for various game events.
- **Input Handling**: The game engine supports keyboard input, including input chords (multi-key presses), for controlling entities and performing other actions in the game.
- **Rendering**: The game engine allows constant and proportional scaling of game objects and rendering game states with customizable frame rates. Additionally, individual clients can move the camera to focus on different parts of the game world.
- **Timeline**: The game engine supports a timeline for managing game events with controllable speed and the ability to pause, resume the game.
- **Multiplayer**: The game engine supports multiplayer gameplay with multiple clients (separate processes) interacting with the game world simultaneously.
- **Multi-threading**: The game engine uses multi-threading to handle networking and rendering in separate threads.
- **Replay System**: The game engine supports a replay system that records and replays a portion of the game client-side.
- **Side-scrolling**: The game engine supports side-scrolling gameplay with a camera that follows the player character.
- **Zones**: The game engine supports multiple spawn and death zones in the game world.

## Screenshots

- Multiplayer ![Multiplayer](Screenshots/multiplayer.png)
- Zones ![Zones](Screenshots/zones.png)
- Snake ![Example Game - Snake](Screenshots/snake.png)
- Brick Breaker ![Example Game - Brick Breaker](Screenshots/brickbreaker.png)

## Common Keybindings

- `Arrow keys` - Movement of controllable entities.
- `esc` / `q` - Quit the game

## Attribution

We used following resources for development:

- SDL2 Documentation: [SDL Wiki](https://wiki.libsdl.org/SDL2/FrontPage)
- ZeroMQ Documentation: [ZeroMQ Wiki](http://wiki.zeromq.org/)
- SDL Game Development Tutorial Series: [YouTube Playlist](https://www.youtube.com/watch?v=FxCC9Ces1Yg&list=PLSPw4ASQYyymu3PfG9gxywSPghnSMiOAW&index=2)
- SDL2 with CMake: [StackOverflow Post](https://stackoverflow.com/questions/28395833/using-sdl2-with-cmake)
- Singleton Design Pattern: [StackOverflow Post](https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern)
- Json Library: [nlohmann's JSON Library](https://github.com/nlohmann/json)
- Class templates (with explicit instantiation): [cppreference](https://en.cppreference.com/w/cpp/language/class_template)
- Type Erasure: [Blog Post](https://davekilian.com/cpp-type-erasure.html), [cplusplus Article](https://cplusplus.com/articles/oz18T05o/)
- Curiously Recurring Template Pattern (CRTP): [cppreference](https://en.cppreference.com/w/cpp/language/crtp)

## Sample Code

### Snake Game

```cpp
#include <iostream>
#include <TypedEventHandler.h>
#include <thread>

#include "GameEngine.h"
#include "Entity.h"
#include "InputEvent.cpp"

enum class Direction
{
    LEFT,
    RIGHT,
    UP,
    DOWN
};

auto currentDirection = Direction::LEFT;
Entity *food = nullptr;

void set_food(const std::deque<std::shared_ptr<Entity>> &snake, std::shared_ptr<Entity> &food)
{
    if (food)
        return;

    float currentX = 0, currentY = 0;
    std::set<std::pair<float, float>> restrictedPositions = {{0.0f, 0.0f}};

    for (const auto &entity : snake)
    {
        restrictedPositions.insert({entity->getPosition().x, entity->getPosition().y});
    }

    while (restrictedPositions.find({currentX, currentY}) != restrictedPositions.end())
    {
        currentX = static_cast<float>((rand() % 75) * 10);
        currentY = static_cast<float>((rand() % 75) * 10);
    }

    food = std::make_unique<Entity>(Position{currentX, currentY}, Size{10, 10}, SDL_Color{255, 0, 0, 255});
}

bool check_collision(const std::deque<std::shared_ptr<Entity>> &snake)
{
    auto head = snake[0].get();

    for (int i = 1; i < snake.size(); i++)
    {
        if (CollisionSystem::hasCollisionRaw(head, snake[i].get()))
        {
            // Snake has collided with itself
            return true;
        }
    }

    if (head->getPosition().x < 0 || head->getPosition().x > 750 || head->getPosition().y < 0 || head->getPosition().y > 750)
    {
        // Snake has collided with the wall
        return true;
    }

    return false;
}

void kill_snake(std::deque<std::shared_ptr<Entity>> &snake)
{
 for (const auto &segment : snake)
 {
     segment->setColor(SDL_Color{255, 0, 0, 255});
 }
}

void move_snake(std::deque<std::shared_ptr<Entity>> &snake, std::shared_ptr<Entity> &food)
{
    bool remove_tail = true;

    // Check if snake has eaten the food
    if (CollisionSystem::hasCollisionRaw(snake.front().get(), food.get()))
    {
        food.reset();
        remove_tail = false;
    }

    // Calculate new head position
    auto newHeadPos = Position(snake.front()->getPosition());
    switch (currentDirection)
    {
    case Direction::LEFT:
        newHeadPos.x -= 10;
        break;
    case Direction::RIGHT:
        newHeadPos.x += 10;
        break;
    case Direction::UP:
        newHeadPos.y -= 10;
        break;
    case Direction::DOWN:
        newHeadPos.y += 10;
        break;
    }

    // Create new head and add it to the front
    snake.push_front(std::make_unique<Entity>(newHeadPos, Size{10, 10}, SDL_Color{0, 255, 0, 255}));

    if (remove_tail)
    {
        snake.pop_back();
    }
}

bool compute_next_state(std::deque<std::shared_ptr<Entity>> &snake, std::shared_ptr<Entity> &food)
{
    // Check if the snake has collided
    if (check_collision(snake))
    {
        // Game over
        return false;
    }

    // Move the snake food
    set_food(snake, food);

    // Compute the new position of the snake
    move_snake(snake, food);

    return true;
}

int main(int argc, char **argv)
{
    GameEngine gameEngine("Snake", 750, 750, Mode::SINGLE_PLAYER);
    std::deque<std::shared_ptr<Entity>> snake;
    std::vector<std::shared_ptr<Entity>> entities;
    std::shared_ptr<Entity> food;
    int cycles = 0;
    bool game_over = false;

    // Create initial snake of length 3
    for (int i = 0; i < 3; i++)
    {
        snake.push_back(std::make_unique<Entity>(
            Position{static_cast<float>(300 + i * 10), 300},
            Size{10, 10},
            SDL_Color{0, 255, 0, 255}));
    }

    entities.reserve(snake.size());
    for (const auto &segment : snake)
    {
        entities.push_back(segment);
    }

    std::vector<Entity *> rawPtrVector;

    rawPtrVector.reserve(entities.size());
    for (const auto &shared : entities)
    {
        rawPtrVector.push_back(shared.get());
    }

    if (!gameEngine.initialize(rawPtrVector))
    {
        return -1;
    }

    gameEngine.disableCollisionHandling();
    gameEngine.setEntities(entities);

    keyBinding esc = {SDL_SCANCODE_ESCAPE};
    keyBinding quit = {SDL_SCANCODE_Q};
    keyBinding left = {SDL_SCANCODE_LEFT};
    keyBinding right = {SDL_SCANCODE_RIGHT};
    keyBinding up = {SDL_SCANCODE_UP};
    keyBinding down = {SDL_SCANCODE_DOWN};

    const EventHandler inputHandler = TypedEventHandler<InputEvent>([&gameEngine, esc, quit, left, right, up, down](const InputEvent *event)
                                                                    {
        const std::set<SDL_Scancode> b = event->getBinding();

        if (b == esc || b == quit) gameEngine.setGameState(GameState::EXIT);
        else if (b == left && currentDirection != Direction::RIGHT) currentDirection = Direction::LEFT;
        else if (b == right && currentDirection != Direction::LEFT) currentDirection = Direction::RIGHT;
        else if (b == up && currentDirection != Direction::DOWN) currentDirection = Direction::UP;
        else if (b == down && currentDirection != Direction::UP) currentDirection = Direction::DOWN; });

    gameEngine.getEventManager()->registerHandler(EventType::Input, inputHandler);

    gameEngine.getInputManager()->bind(quit);
    gameEngine.getInputManager()->bind(esc);
    gameEngine.getInputManager()->bind(left);
    gameEngine.getInputManager()->bind(right);
    gameEngine.getInputManager()->bind(up);
    gameEngine.getInputManager()->bind(down);

    gameEngine.setOnCycle([&]()
                          {
        cycles += 1;
        if (cycles % 5 != 0) return;

        game_over = !compute_next_state(snake, food);
        entities.clear();

        if (game_over) {
            kill_snake(snake);
            for (const auto& segment : snake) {
                entities.push_back(std::make_shared<Entity>(*segment)); // Create shared_ptr copies
            }
            gameEngine.setEntities(entities);
            //Sleep for 3 seconds
            std::this_thread::sleep_for(std::chrono::seconds(3));

            gameEngine.setGameState(GameState::EXIT);
            return;
        }

        for (const auto& segment : snake) {
            entities.push_back(std::make_shared<Entity>(*segment)); // Create shared_ptr copies
        }

        if (food) entities.push_back(std::make_shared<Entity>(*food));

        gameEngine.setEntities(entities);
                          });

    gameEngine.run();
    gameEngine.shutdown();

    return 0;
}
```
