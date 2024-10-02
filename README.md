# GameEngine

- GitHub repository - [GameEngine](https://github.ncsu.edu/yrajapa/GameEngine)

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
- Install 0mq - `brew install cppzmq zeromq`

## Building and running
- Note: Pre-built binaries can be found at `./build`
- 
- To compile and run the game: `cmake -S . -B build && cmake --build build`
- The `build` directory will now contain the binaries
- Run the server executable first, then run 1 or more client executables

</details>

## Keybindings

- `s` - Toggle between Constant and Proportional scaling methods.
- `Arrow keys` - Movement of controllable entities.
- `esc` / `q` - Quit the game

## Attribution

We used following resources for development:

- SDL2 Documentation: [SDL Wiki](https://wiki.libsdl.org/SDL2/FrontPage)
- SDL Game Development Tutorial Series: [YouTube Playlist](https://www.youtube.com/watch?v=FxCC9Ces1Yg&list=PLSPw4ASQYyymu3PfG9gxywSPghnSMiOAW&index=2)
- SDL2 with CMake: [StackOverflow Post](https://stackoverflow.com/questions/28395833/using-sdl2-with-cmake)
- Singleton Design Pattern: [StackOverflow Post](https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern)

