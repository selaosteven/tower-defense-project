# PROJET PROGRAMATION AVANCÉE
## STEVEN SELAO && VICTOR GIERLOWSKI

## Description
This project is a Tower Defense game developed in C++ as part of an academic assignment. The main objective was to build a fully functional Tower Defense experience while respecting strict constraints, including the prohibition of raw pointers in the core game logic. The entire game is powered by a custom graphics engine built on top of SDL2.

## Implementation Overview

### 💡 Idea behind the game

The game was designed from the start to be highly modular, allowing new behaviors, stats, or visual elements to be added easily without modifying the core engine. All gameplay entities such as towers and enemies load their configuration from JavaScript‑like data files, which define their statistics and upgrade trees. The map itself is read from an ASCII file, making level creation simple and flexible.

### 🏯 Towers 
For tower visuals, we implemented our own geometric primitives (circles, rectangles, triangles, octagons, etc.) generated through mathematical calculations and rendered using SDL. These primitives are combined to create unique tower designs, and their composition is defined through external configuration files.

### 👾 Ennemies
Enemies are represented using ASCII characters, each corresponding to a small pixel‑art Pokémon drawing. They follow a predefined path extracted from the ASCII map, giving the game a unique and playful identity.

### ⚡ Optimization
To optimize performance, especially for tower targeting, we integrated a quadtree spatial partitioning system. Instead of checking every enemy on the map, each tower only queries enemies located in the relevant quadtree regions. This significantly reduces computation and keeps the game efficient even with many entities on screen.



### Prepare the build 
(default to release use -CMAKE_BUILD_TYPE=Debug for debug)
```sh
mkdir build
cd build
cmake ..
```

### Compile
```sh
make -j
```

### Run
```sh
./TowerDefense
```
Or with the path to a map file
```sh
./TowerDefense ../src/Ressources/map1.txt
```
