#include <flecs.h>
#include "tigr.h"

Tigr *screen, *image;

/* Component types */
typedef struct Vector2D {
    float x;
    float y;
} Vector2D;

typedef Vector2D Position;
typedef Vector2D WorldPosition;
typedef Vector2D Velocity;

/* Implement a simple move system */
void Move(ecs_rows_t *rows) {
    /* Get the two columns from the system signature */
    ECS_COLUMN(rows, Position, p, 1);
    ECS_COLUMN(rows, Velocity, v, 2);

    printf("moving\n");
    for (int i = 0; i < rows->count; i ++) {
        p[i].x += v[i].x * rows->delta_time;
        p[i].y += v[i].y * rows->delta_time;
    }
}

/* Implement a system that transforms world coordinates hierarchically. If the 
 * CASCADE column is set, it points to the world coordinate of the parent. This
 * will be used to then transform Position to WorldPosition of the child.
 * If the CASCADE column is not set, the system matched a root. In that case,
 * just assign the Position to the WorldPosition. */
void Transform(ecs_rows_t *rows) {
    /* Get the two columns from the system signature */
    ECS_COLUMN(rows, WorldPosition, parent_wp, 1);
    ECS_COLUMN(rows, WorldPosition, wp, 2);
    ECS_COLUMN(rows, Position, p, 3);

    if (!parent_wp) {
        printf("transforming parent\n");
        for (int i = 0; i < rows->count; i ++) {
            wp[i].x = p[i].x;
            wp[i].y = p[i].y;
        }
    } else {
        printf("transforming childs\n");
        for (int i = 0; i < rows->count; i ++) {
            wp[i].x = parent_wp->x + p[i].x;
            wp[i].y = parent_wp->y + p[i].y;
        }
    }
}

void Render(ecs_rows_t *rows) {
    ECS_COLUMN(rows, WorldPosition, p, 1);

    for (int i = 0; i < rows->count; i ++) {
        tigrBlitAlpha(screen, image, p[i].x, p[i].y, 0, 0, image->w, image->h, 1);
    }
    printf("Rendering.\n");
}

void UpdateScreen() {
    tigrUpdate(screen);
    tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));
    printf("Updating screen\n");
}

int main(int argc, char *argv[]) {
    /* Create the world, pass arguments for overriding the number of threads,fps
     * or for starting the admin dashboard (see flecs.h for details). */
    ecs_world_t *world = ecs_init_w_args(argc, argv);

    screen = tigrWindow(640, 480, "Hierarchy test", TIGR_2X);
    image = tigrLoadImage("assets/squinkle.png");

    /* Register components */
    ECS_COMPONENT(world, Position);
    ECS_COMPONENT(world, WorldPosition);
    ECS_COMPONENT(world, Velocity);

    /* Move entities with Position and Velocity */
    ECS_SYSTEM(world, Move, EcsOnUpdate, Position, Velocity);

    /* Transform local coordinates to world coordinates. A CASCADE column
     * guarantees that entities are evaluated breadth-first, according to the
     * hierarchy. This system will depth-sort based on parents that have the
     * WorldPosition component. */
    ECS_SYSTEM(world, Transform, EcsOnUpdate, CASCADE.WorldPosition, WorldPosition, Position);
    ECS_SYSTEM(world, Render, EcsOnUpdate, WorldPosition);

    /* Create root of the hierachy which moves around */
    ECS_ENTITY(world, Root, WorldPosition, Position, Velocity);
    ecs_set(world, Root, Position, {0, 0});
    ecs_set(world, Root, Velocity, {10, 5});

    /* Create children that don't move and are relative to the parent */
    ECS_ENTITY(world, Child1, WorldPosition, Position, CHILDOF | Root);
    ecs_set(world, Child1, Position, {100, 100});

    ECS_ENTITY(world, GChild1, WorldPosition, Position, CHILDOF | Child1);
    ecs_set(world, GChild1, Position, {10, 10});

    ECS_ENTITY(world, Child2, WorldPosition, Position, CHILDOF | Root);
    ecs_set(world, Child2, Position, {20, 20});

    ECS_ENTITY(world, GChild2, WorldPosition, Position, CHILDOF | Child2);
    ecs_set(world, GChild2, Position, {200, 200});

    /* Set target FPS for main loop to 1 frame per second */
    ecs_set_target_fps(world, 1);

    printf("Application move_system is running, press CTRL-C to exit...\n");
    tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));

    ECS_SYSTEM(world, UpdateScreen, EcsPostUpdate, 0);

    /* Run systems */
    while (!tigrKeyDown(screen, TK_ESCAPE) && ecs_progress(world, 0)){
        printf("######### mainLoop Called #######\n");
    };

    tigrFree(screen);
    tigrFree(image);
    /* Cleanup */
    return ecs_fini(world);
}