#include <math.h>
#include <stdio.h>
#include <raylib.h>

#include "stb_ds.h"

const int WINDOW_WIDTH = 600;
const int WINDOW_HEIGHT = 1000;
const Color BG_COLOR = (Color) { .r = 210, .g = 200, .b = 190, .a = 255 };
const int MOVEMENT_THRESHOLD = 12;

typedef enum {
    FORWARD_DIRECTION,
    BACKWARD_DIRECTION
} Direction;

typedef enum {
    X_AXIS,
    Z_AXIS
} Axis;

typedef struct {
    float speed;
    Direction direction;
    Axis axis;
} Movement;

typedef struct {
    size_t index;
    Vector3 position;
    Vector3 size;
    Color color;
    int colorOffset;
    Movement movement;
} Block;

const Block default_block = (Block) {
    .index = 0,
    .position = (Vector3) { .x = 0, .y = 0, .z = 0 },
    .size = (Vector3) { .x = 10, .y = 2, .z = 10 },
    .color = (Color) { .r = 150, .g = 150, .b = 150, .a = 255 },
    .colorOffset = 0,
    .movement = (Movement) {
        .speed = 0,
        .direction = FORWARD_DIRECTION,
        .axis = X_AXIS
    }
};

typedef enum {
    READY_STATE,
    PLAYING_STATE,
    GAME_OVER_STATE
} GameState;

typedef struct {
    GameState state;
    Block *placed_blocks;
    Block *previous_block;
    Block current_block;
} Game;

void InitGame(Game *game) {
    game->state = READY_STATE;
    game->placed_blocks = nullptr;
    game->current_block = default_block;
    game->current_block.colorOffset = GetRandomValue(0, 100);

    arrpush(game->placed_blocks, default_block);
    game->previous_block = &game->placed_blocks[0];
}

void DrawBlock(const Block *block) {
    DrawCube(block->position, block->size.x, block->size.y, block->size.z, block->color);
    DrawCubeWires(block->position, block->size.x, block->size.y, block->size.z, BLACK);
}

void DrawPlacedBlocks(Game *game) {
    size_t blocks_len = arrlen(game->placed_blocks);

    for (size_t i = 0; i < blocks_len; i++) {
        Block *block = &game->placed_blocks[i];
        DrawBlock(block);
    }
}

Block CreateMovingBlock(Game *game) {
    Block *target = game->previous_block;

    Axis axis = target->movement.axis == X_AXIS ? Z_AXIS : X_AXIS;
    Direction direction = GetRandomValue(0, 1) == 0 ? FORWARD_DIRECTION : BACKWARD_DIRECTION;

    Vector3 position = target->position;
    position.y += target->size.y;

    if (axis == X_AXIS) {
        position.x = (direction == FORWARD_DIRECTION ? -1 : 1) * MOVEMENT_THRESHOLD;
    } else {
        position.z = (direction == FORWARD_DIRECTION ? -1 : 1) * MOVEMENT_THRESHOLD;
    }

    size_t index = target->index + 1;
    int offset = target->colorOffset + index;
    float r = sinf(0.3 * offset) * 55 + 200;
    float g = sinf(0.3 * offset + 2) * 55 + 200;
    float b = sinf(0.3 * offset + 4) * 55 + 200;

    return (Block) {
        .index = index,
        .position = position,
        .size = target->size,
        .color = (Color) { r, g, b, 255 },
        .colorOffset = target->colorOffset,
        .movement = (Movement) {
            .speed = 12 + index * 0.5,
            .direction = direction,
            .axis = axis
        }
    };
}

bool PlaceBlock(Game *game) {
    Block *current = &game->current_block;
    Block *target = game->previous_block;

    bool isXAxis = current->movement.axis == X_AXIS;
    float currentPosition = isXAxis ? current->position.x : current->position.z;
    float targetPosition = isXAxis ? target->position.x : target->position.z;
    [[maybe_unused]]float currentSize = isXAxis ? current->size.x : current->size.z;
    float targetSize = isXAxis ? target->size.x : target->size.z;

    float delta = currentPosition - targetPosition;
    float overlay = targetSize - fabs(delta);

    if (overlay < 0.1) {
        return false;
    }

    bool isPerfectOverlay = fabs(delta) < 0.3;
    if (isPerfectOverlay) {
        if (isXAxis) {
            current->size.x = target->size.x;
            current->position.x = target->position.x;
        } else {
            current->size.z = target->size.z;
            current->position.z = target->position.z;
        }
    } else {
        if (isXAxis) {
            current->size.x = overlay;
            current->position.x = targetPosition + delta / 2;
        } else {
            current->size.z = overlay;
            current->position.z = targetPosition + delta / 2;
        }
    }

    arrpush(game->placed_blocks, game->current_block);
    size_t len = arrlen(game->placed_blocks);
    game->previous_block = &game->placed_blocks[len-1];

    return true;
}

void UpdateGameState(Game *game) {
    bool inputPressed = IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    switch (game->state) {
        case READY_STATE: {
            if (inputPressed) {
                game->state = PLAYING_STATE;
                game->current_block = CreateMovingBlock(game);
            }
            break;
        }

        case PLAYING_STATE: {
            if (inputPressed) {
                bool success = PlaceBlock(game);
                if (success) {
                    game->current_block = CreateMovingBlock(game);
                } else {
                    game->state = GAME_OVER_STATE;
                }
            }
            break;
        }

        case GAME_OVER_STATE: {
            if (inputPressed) {
                arrfree(game->placed_blocks);
                InitGame(game);
                game->state = PLAYING_STATE;
                game->current_block = CreateMovingBlock(game);
            }
            break;
        }
    }
}

void UpdateCameraPosition(const Game *game, Camera3D *camera) {
    size_t placed_blocks_len = arrlen(game->placed_blocks);
    camera->position.y = 50 + 2 * placed_blocks_len;
    camera->target.y = 2 * placed_blocks_len;
}

void DrawCurrentBlock(Game *game) {
    if (game->state != PLAYING_STATE) {
        return;
    }

    DrawBlock(&game->current_block);
}

void DrawGameStartOverlay(const Game *game) {
    if (game->state != READY_STATE) {
        return;
    }

    const char *title = "START GAME";
    int fontSize = 60;

    int screenWidth = GetScreenWidth();
    int textSize = MeasureText(title, fontSize);
    int position = (screenWidth - textSize) / 2;
    DrawText(title, position, 50, fontSize, DARKGRAY);
}

void DrawGameScore(Game *game) {
    if (game->state != PLAYING_STATE) {
        return;
    }

    const char *title = "100";
    int fontSize = 60;

    int screenWidth = GetScreenWidth();
    int textSize = MeasureText(title, fontSize);
    int position = (screenWidth - textSize) / 2;
    DrawText(title, position, 50, fontSize, DARKGRAY);
}

void DrawGameOverOverlay(Game *game) {
    if (game->state != GAME_OVER_STATE) {
        return;
    }

    const char *title = "GAME OVER";
    int fontSize = 60;

    int screenWidth = GetScreenWidth();
    int textSize = MeasureText(title, fontSize);
    int position = (screenWidth - textSize) / 2;
    DrawText(title, position, 50, fontSize, DARKGRAY);
}

void UpdateCurrentBlock(Game *game, float dt) {
    if (game->state != PLAYING_STATE) {
        return;
    }

    Block *block = &game->current_block;
    int direction = block->movement.direction == FORWARD_DIRECTION ? 1 : -1;

    float *axisPosition = block->movement.axis == X_AXIS ? &block->position.x : &block->position.z;
    *axisPosition += direction * block->movement.speed * dt;

    if (fabs(*axisPosition) >= MOVEMENT_THRESHOLD) {
        block->movement.direction = block->movement.direction == FORWARD_DIRECTION ? BACKWARD_DIRECTION : FORWARD_DIRECTION;
        *axisPosition = fmax(fmin(MOVEMENT_THRESHOLD, *axisPosition), -MOVEMENT_THRESHOLD);
    }
}

int main(void) {
    Game game;
    InitGame(&game);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Tower Blocks");
    SetTargetFPS(60);

    Camera3D camera = (Camera3D) {
        .position = (Vector3) { .x = 50, .y = 50, .z = 50 },
        .target = (Vector3) { .x = 0, .y = 0, .z = 0 },
        .up = (Vector3) { .x = 0, .y = 1, .z = 0 },
        .fovy = 60,
        .projection = CAMERA_ORTHOGRAPHIC
    };

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        UpdateGameState(&game);
        UpdateCameraPosition(&game, &camera);
        UpdateCurrentBlock(&game, dt);

        BeginDrawing();
            ClearBackground(BG_COLOR);
            BeginMode3D(camera);
                DrawPlacedBlocks(&game);
                DrawCurrentBlock(&game);
            EndMode3D();

            DrawGameStartOverlay(&game);
            DrawGameScore(&game);
            DrawGameOverOverlay(&game);

            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
