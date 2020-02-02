#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif
#include "tigr.h"
#include <stdio.h>
#include "flecs.h"
#include <math.h>

#define MATRIX_SIZE_WIDTH 200
#define MATRIX_SIZE_HEIGHT 200

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT  600

#define SCREEN_CENTER_X SCREEN_WIDTH / 2
#define SCREEN_CENTER_Y SCREEN_HEIGHT / 2

typedef float PositionX;
typedef float PositionY;
typedef float PositionZ;

typedef struct Position {
    float x;
    float y;
    float z;
} Position;

typedef int32_t Speed;

Tigr *screen;

void Render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, PositionX, x, 1);
    ECS_COLUMN(rows, PositionY, y, 2);
    ECS_COLUMN(rows, PositionZ, z, 3);
    tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));
    TPixel px = { 0xff, 0xff, 0xff, 0xff };
    for (int i = 0; i < rows->count; i ++) {
        tigrPlot(screen, x[i] * z[i], y[i] * z[i], px);
    }
    tigrUpdate(screen);
}

float distance(float x, float y) {
    return sqrtf(powf(SCREEN_CENTER_X - x, 2) + powf(SCREEN_CENTER_Y - y, 2));
}

void UpdateParticle(ecs_rows_t *rows){
    ECS_COLUMN(rows, PositionX, x, 1);
    ECS_COLUMN(rows, PositionY, y, 2);

    static float phase = 0;
    phase += 10 * rows->delta_time;
    const int paddingX = 2;
    const int paddingY = 2;
    const int offsetX = (SCREEN_WIDTH - MATRIX_SIZE_WIDTH * paddingX) / 2;
    const int offsetY = (SCREEN_HEIGHT - MATRIX_SIZE_HEIGHT * paddingY) / 2;

    float phaseOffset = 0;


    for(int col = 0; col < MATRIX_SIZE_WIDTH; col++){
        for(int row = 0; row < MATRIX_SIZE_HEIGHT; row++){
//            phaseOffset += .3;
            phaseOffset = distance(x[col * MATRIX_SIZE_HEIGHT + row], y[col * MATRIX_SIZE_HEIGHT + row]) * .1;
            x[col * MATRIX_SIZE_HEIGHT + row] = col * paddingX + offsetX + sin(phase + phaseOffset) * 5;
            y[col * MATRIX_SIZE_HEIGHT + row] = row * paddingY + offsetY + cos(phase + phaseOffset) * 5;
        }
    }

}

void InitMatrix(ecs_rows_t *rows){
    ECS_COLUMN(rows, PositionX, x, 1);
    ECS_COLUMN(rows, PositionY, y, 2);
    ECS_COLUMN(rows, PositionZ, z, 3);

    const int paddingX = 10;
    const int paddingY = 10;
    const int offsetX = (SCREEN_WIDTH - MATRIX_SIZE_WIDTH * paddingX) / 2;
    const int offsetY = (SCREEN_HEIGHT - MATRIX_SIZE_HEIGHT * paddingY) / 2;

    float zDistance = 1.0;

    for(int col = 0; col < MATRIX_SIZE_WIDTH; col++){
        for(int row = 0; row < MATRIX_SIZE_HEIGHT; row++){
            x[col * MATRIX_SIZE_HEIGHT + row] = col * paddingX + offsetX;
            y[col * MATRIX_SIZE_HEIGHT + row] = row * paddingY + offsetY;
            z[col * MATRIX_SIZE_HEIGHT + row] = zDistance;
        }
    }

}
ecs_world_t *world;
void game_update(void) {
    ecs_progress(world, 0);
}

int main(int argc, char *argv[]) {

    world = ecs_init_w_args(argc, argv);

    /* Register components and systems */
    ECS_COMPONENT(world, PositionX);
    ECS_COMPONENT(world, PositionY);
    ECS_COMPONENT(world, PositionZ);

    ECS_SYSTEM(world, Render, EcsOnUpdate, PositionX, PositionY, PositionZ);

    ECS_SYSTEM(world, UpdateParticle, EcsOnUpdate, PositionX, PositionY);

    ECS_SYSTEM(world, InitMatrix, EcsOnAdd, PositionX, PositionY, PositionZ);

    ECS_TYPE(world, DotMatrix, PositionX, PositionY, PositionZ);

    ecs_entity_t e = ecs_new_w_count(world, DotMatrix, MATRIX_SIZE_WIDTH * MATRIX_SIZE_HEIGHT);

    /* Limit application to 60 FPS */
    ecs_set_target_fps(world, 60);

    screen = tigrWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello", TIGR_2X);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(game_update, 0, 1);
#else
    /* Progress world in main loop (invokes Move system) */
    while (!tigrClosed(screen) && !tigrKeyDown(screen, TK_ESCAPE)) {
        ecs_progress(world, 0);
    }
#endif
    tigrFree(screen);
    return ecs_fini(world);
}