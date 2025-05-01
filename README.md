# 🧭A* Pathfinding Visualizer
This application implements the `A*` (A-Star) algorithm to find the shortest path from a start point to a destination on a customizable grid. You can adjust the grid's width, height, and choose between 4-directional or 8-directional movement.
Built in C, it uses [raylib](https://github.com/raysan5/raylib) for graphics and [raygui](https://github.com/raysan5/raygui) for GUI components.

## ✨Features
* Visualization of the `A*` algorithm.
* Adjustable grid dimensions (width and height).
* Choice of navigation mode:
  * 4-directional (up, down, left, right)
  * 8-directional (including diagonals)
* Click to set start and end points.
* Support for placing obstacles.

## 📚A* algorithm
`A*` (pronounced "A-star") is a graph traversal and pathfinding algorithm that is used in many fields of computer science due to its completeness, optimality, and optimal efficiency. Given a weighted graph, a source node and a goal node, the algorithm finds the shortest path (with respect to the given weights) from source to goal.

## 🚀How to build
```c
git clone https://github.com/EmulRyo/AStar.git
cd AStar
cmake -B build && cmake --build build
```
