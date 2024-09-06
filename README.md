# GameEngine

## Requirements: Windows

1. Visual Studio Community 2022
2. SDL2 (included)

## How to Run: Windows

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
