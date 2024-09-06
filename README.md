# GameEngine

- GitHub repository - [GameEngine](https://github.ncsu.edu/yrajapa/GameEngine])

<details>
  <summary>Windows</summary>

## Requirements

1. Visual Studio Community 2022
2. SDL2 (included)

## How to Run

### Option 1: Run within Visual Studio

1. Open the solution (`GameEngine.sln`) in Visual Studio Community 2022.
2. In the run configurations dropdown:
   - Select **'RunEngine'** to run the game engine.
   - Select **'RunGame'** to run the game.
3. Click the **'Local Windows Debugger'** button to run the selected configuration within Visual Studio.

### Option 2: Run Manually

1. Right-click on the 'GameEngine' solution inside the Solution Explorer in Visual Studio.
2. Select **'Rebuild Solution'**.
3. To run the game:
   - Navigate to `GameEngine/x64/RunGame`.
   - Double-click the **Game.exe** file.
4. To run the game engine:
   - Navigate to `GameEngine/x64/RunEngine`.
   - Double-click the **GameEngine.exe** file.

**Note:**  
Ensure that the `SDL2.dll` file is present in the directories where the `.exe` files are located. This file is already included; do not remove it.

</details>

<details>
  <summary>MacOS (Silicon)</summary>

## Requirements
- Install SDL2: `brew install sdl2`
- Install CMake: `brew install cmake`
   
## Building and running
- Note: Pre-built binary can be found at `./build/GameEngine` (running this still requires SDL2 to be installed)
- To compile and run the game: `cmake -S . -B build && cmake --build build && ./build/GameEngine`
- In `CMakeLists.txt`, if `GameEngine/main.cpp` is included as an executable inside `add_executable()`, then the game engine itself will be executed
- If you wish to run the actual game, include `Game/main.cpp` instead of `GameEngine/main.cpp`

</details>

## Keybindings

- `s` - Toggle between Constant and Proportional scaling methods.
- `Arrow keys` - Movement of controllable entities.
- `esc` / `q` - Quit the game
