#include <stdio.h>
#include <flecs.h>

void system_evens(ecs_rows_t *rows) {
    printf("evens\n");
    ECS_COLUMN_COMPONENT(rows, tag_to_remove, 1);
    ECS_COLUMN_COMPONENT(rows, tag_to_add, 2);
    for (int i = 0; i < rows->count; i++) {
        ecs_remove(rows->world, rows->entities[i], tag_to_remove);
        ecs_add(rows->world, rows->entities[i], tag_to_add);
    }
}

void system_odds(ecs_rows_t *rows) {
    printf("ods\n");
    ECS_COLUMN_COMPONENT(rows, tag_to_remove, 1);
    ECS_COLUMN_COMPONENT(rows, tag_to_add, 2);
    for (int i = 0; i < rows->count; i++) {
        ecs_remove(rows->world, rows->entities[i], tag_to_remove);
        ecs_add(rows->world, rows->entities[i], tag_to_add);
    }
}

void system_test_update(ecs_rows_t *rows) {
    printf("updating\n");
}

int main(int argc, char *argv[]) {
    ecs_world_t *world = ecs_init_w_args(argc, argv);
    ECS_TAG(world, tag_evens);
    ECS_TAG(world, tag_odds);
    ECS_SYSTEM(world, system_test_update, EcsOnUpdate, 0);

    ECS_SYSTEM(world, system_evens, EcsOnUpdate, tag_evens, .tag_odds);

    ECS_SYSTEM(world, system_odds, EcsOnUpdate, tag_odds, .tag_evens);


    int break_condition = 100;

    ECS_TYPE(world, Dummy, tag_evens);
    ecs_new_w_count(world, Dummy, 1);


    while (ecs_progress(world, 0) && break_condition--);

    ecs_fini(world);
}