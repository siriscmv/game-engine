# GameEngine

- GitHub repository - [GameEngine](https://github.ncsu.edu/yrajapa/GameEngine)

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
- ZeroMQ Documentation: [ZeroMQ Wiki](http://wiki.zeromq.org/)
- SDL Game Development Tutorial Series: [YouTube Playlist](https://www.youtube.com/watch?v=FxCC9Ces1Yg&list=PLSPw4ASQYyymu3PfG9gxywSPghnSMiOAW&index=2)
- SDL2 with CMake: [StackOverflow Post](https://stackoverflow.com/questions/28395833/using-sdl2-with-cmake)
- Singleton Design Pattern: [StackOverflow Post](https://stackoverflow.com/questions/1008019/how-do-you-implement-the-singleton-design-pattern)
- Json Library [nlohmann's JSON Library] (https://github.com/nlohmann/json)

