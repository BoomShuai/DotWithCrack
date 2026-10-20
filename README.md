# DotWithCrack

Several small C++ graphics and physics experiments, ranging from SDL collision demos to a multithreaded ball-and-piston simulation. The repository is a collection of learning projects rather than one unified application; build and run the example that matches the topic you want to explore.

## Projects

| Directory | What it demonstrates | Main dependencies |
|---|---|---|
| `BallTest/` | SDL rendering, input handling, and particle-style ball animation | SDL2, SDL2_image |
| `BallDistance/` | Circle-circle and circle-rectangle collision checks using distance calculations | SDL2, SDL2_image |
| `BallBoxTest/` | A second SDL collision example focused on balls and a rectangular area | SDL2, SDL2_image |
| `BaseCode/` | A larger simulation with balls, walls, a moving piston, collision messages, and worker threads | Visual Studio C++, SDL2 |

The root-level bitmap files are sample textures used by the SDL examples. Each example has its own `main` function, so compile one project at a time.

## Build and run

The checked-in Visual Studio project is [`BaseCode/MaDing.sln`](BaseCode/MaDing.sln). It targets Visual Studio 2022 (toolset v143) and SDL2. The project file currently contains machine-specific SDL include and library paths; update those paths for your installation before building. Copy the matching SDL2 runtime DLL beside the built executable if Visual Studio does not do so automatically.

The `BallTest`, `BallDistance`, and `BallBoxTest` folders are standalone examples and do not include Visual Studio solution files. Create or configure a project with SDL2 and SDL2_image include/library paths, then add the `.cpp` files for that single example. Some source files also contain absolute paths from the original development machine; update image paths to relative paths such as `dot.bmp` before running elsewhere.

## Topics to explore

- Representing moving objects and updating their position over time.
- Detecting collisions between circles and between circles and rectangles.
- Separating simulation state from SDL rendering and input handling.
- Coordinating simulation work across threads in the larger `BaseCode` example.

## Repository layout

```text
.
├── BallTest/       # SDL particle and ball rendering experiment
├── BallDistance/   # Distance-based collision checks
├── BallBoxTest/    # Ball and box collision experiment
├── BaseCode/       # Larger multithreaded simulation and Visual Studio solution
└── *.bmp           # Example textures
```

This repository records experiments and code studies. SDL setup and portability work may be needed before the examples build on a different machine.
