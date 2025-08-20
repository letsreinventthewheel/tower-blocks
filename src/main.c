#include <stdio.h>
#include <raylib.h>

#include "stb_ds.h"

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 1000;
const Color BG_COLOR = (Color) { .r = 210, .g = 200, .b = 190, .a = 255 };

typedef struct {
    Vector3 position;
    Vector3 size;
    Color color;
} Block;

const Block default_block = (Block) {
    .position = (Vector3) { .x = 0, .y = 0, .z = 0 },
    .size = (Vector3) { .x = 10, .y = 2, .z = 10 },
    .color = (Color) { .r = 150, .g = 150, .b = 150, .a = 255 },
};

typedef struct {
    Block *placed_blocks;
} Game;

void InitGame(Game *game) {
    game->placed_blocks = nullptr;

    arrpush(game->placed_blocks, default_block);
}

void DrawBlock(const Block *block) {
    DrawCube(block->position, block->size.x, block->size.y, block->size.z, block->color);
    DrawCubeWires(block->position, block->size.x, block->size.y, block->size.z, BLACK);
}

void DrawPlacedBlocks(Block *blocks) {
    size_t blocks_len = arrlen(blocks);

    for (size_t i = 0; i < blocks_len; i++) {
        Block *block = &blocks[i];
        DrawBlock(block);
    }
}

void UpdateGameState(Game *game) {
    bool inputPressed = IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    if (inputPressed) {
        Block block = default_block;
        size_t i = arrlen(game->placed_blocks);
        block.position.y = i * 2;
        block.color.r += i * 5;
        block.color.g += i * 5;
        block.color.b += i * 5;
        arrpush(game->placed_blocks, block);
    }
}

void UpdateCameraPosition(const Game *game, Camera3D *camera) {
    size_t placed_blocks_len = arrlen(game->placed_blocks);
    camera->position.y = 50 + 2 * placed_blocks_len;
    camera->target.y = 2 * placed_blocks_len;
}

int main(void) {
    Game game;
    InitGame(&game);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tower Blocks");

    Camera3D camera = (Camera3D) {
        .position = (Vector3) { .x = 50, .y = 50, .z = 50 },
        .target = (Vector3) { .x = 0, .y = 0, .z = 0 },
        .up = (Vector3) { .x = 0, .y = 1, .z = 0 },
        .fovy = 60,
        .projection = CAMERA_ORTHOGRAPHIC
    };

    while (!WindowShouldClose()) {
        UpdateGameState(&game);
        UpdateCameraPosition(&game, &camera);

        BeginDrawing();
            ClearBackground(BG_COLOR);
            BeginMode3D(camera);
                DrawPlacedBlocks(game.placed_blocks);
            EndMode3D();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
