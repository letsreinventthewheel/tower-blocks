# Tower Blocks

Tower Blocks is a 3D block-stacking game written in C with Raylib. It is inspired by games like Stack: a block moves back and forth, the player places it, the overlapping portion becomes the next platform, and the trimmed-off portion falls away.

## Watch the YouTube Series

[Tower Blocks playlist](https://www.youtube.com/playlist?list=PLI7p1zrAYQeW_PnteVmZAkTB3Xvm35MXq)

## What It Does

- Implements a complete block-stacking game loop.
- Alternates block movement between the X and Z axes.
- Calculates overlap in real time and trims blocks after each placement.
- Spawns falling pieces for the portions that miss the tower.
- Tracks score based on the number of successful placements.
- Uses a smooth orthographic camera that follows the tower height.
- Renders blocks with a custom GLSL lighting shader.
- Includes start, playing, game over, and reset states.
- Plays a collapsing tower animation when restarting after game over.

## What You Can Learn

This project is useful if you want to study a compact 3D game built without a full engine:

- Setting up a C23 project with CMake and Raylib.
- Structuring gameplay around explicit game states.
- Modeling moving platforms, placement input, scoring, and restart flow.
- Computing one-dimensional overlap for a 3D stacking mechanic.
- Creating simple physics for falling block fragments.
- Managing dynamic arrays in C with `stb_ds`.
- Rendering custom-transformed cube models.
- Passing model, color, and camera data into GLSL shaders.
- Using interpolation for camera and UI animation.

## Requirements

- C compiler with C23 support.
- CMake 3.25 or newer.
- Raylib installed and discoverable by CMake.

The build is configured with `-Wall`, `-Wextra`, `-Wpedantic`, and `-Werror`.

## Build

```bash
cmake -B build
cmake --build build
```

The executable is built as:

```bash
build/tb
```

## Run

```bash
./build/tb
```

The game expects the shader files to be available at `shaders/lighting_vertex.glsl` and `shaders/lighting_fragment.glsl` relative to the working directory, so run it from the repository root.

## Controls

- `Space` or left mouse button: start the game, place the current block, or restart after game over.
- `Esc` or the window close button: quit.

## Gameplay

Each block moves across the tower until the player places it. If it overlaps the previous block, the overlapped section becomes the next platform and the missed section falls away. If the overlap is almost perfect, the game snaps the block into alignment. If there is not enough overlap, the game ends.

The tower gets harder as it grows because each new block moves faster than the previous one. The camera follows the tower upward so the active block stays in view.

## Technical Notes

### Game State

The main loop updates behavior according to four states:

- `READY_STATE`: waits for the player to start.
- `PLAYING_STATE`: moves the current block and accepts placement input.
- `GAME_OVER_STATE`: shows the game over overlay and waits for restart input.
- `RESETTING_STATE`: removes placed blocks with a cascading collapse animation.

### Block Placement

Placement is handled by comparing the moving block against the previous block on the active axis. The overlap becomes the new block size, and the remainder is converted into a falling block with its own position, velocity, and rotation.

### Rendering

The game renders a generated cube mesh through a custom shader. Each block sets its own transform matrix and color before drawing. The shader applies ambient, diffuse, and specular lighting using the fragment position, normal, and camera position.

## Further Ideas

- Add persistent high score tracking.
- Add sound effects for placement, perfect placement, falling blocks, and game over.
- Replace the debug FPS counter with a polished in-game UI.
- Add difficulty presets or a slower practice mode.

## External Resources

- [Raylib examples](https://www.raylib.com/examples.html)
- [LearnOpenGL: Basic Lighting](https://learnopengl.com/Lighting/Basic-Lighting)
- [stb single-file public domain libraries](https://github.com/nothings/stb)

## Acknowledgements

- Stack and similar mobile block-stacking games for the original gameplay idea.
- Raylib for the rendering, input, and windowing foundation.
- Sean Barrett for the `stb_ds` dynamic array implementation.
