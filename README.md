# Battleship Game 

## Overview
This is a Battleship game developed in C using SDL3 as part of an academic project. The game supports both Player vs Player and Player vs Computer modes on a 14×14 grid.

## Features
- Player vs Player and Player vs AI modes  
- 14×14 game grid  
- Turn-based gameplay system  
- Basic AI with multiple behaviors  
- Modular architecture  
- Save and load functionality  


## Technologies Used
- C Programming Language  
- SDL3 (Simple DirectMedia Layer)  
- Modular programming design  


## Project Structure

### Core
- `main.c` → Entry point, runs the main game loop  

### Modules
- `ai.c / ai.h` → Artificial intelligence (computer opponent)  
- `render.c / render.h` → Graphics rendering using SDL3  
- `random.c / random.h` → Random ship placement  
- `files.c / files.h` → Save/load system (scores)  
- `types.c / types.h` → Data structures and game types  
- `utils.c / utils.h` → Utility/helper functions  

### Other Files
- `SDL3.dll` → SDL3 graphics library  
- `main.exe` → Compiled executable  
- `scores.txt` → Saved scores data  


## How to Run

### 1. Install SDL3
Make sure SDL3 is installed and properly configured on your system.

### 2. Compile
```bash
gcc main.c ai.c files.c random.c render.c types.c utils.c -o battleship -I"C:/SDL3/include" -L"C:/SDL3/lib" -lSDL3
