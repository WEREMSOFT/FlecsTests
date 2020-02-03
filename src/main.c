#include <stdio.h>
#include "flecs.h"

void system_evens(ecs_rows_t *rows){
    printf("evens\n");
    ecs_entity_t my_tag = ecs_get_system_context(rows->world, rows->system);
    ECS_COLUMN_ENTITY(rows, tag_to_remove, 1);
    ECS_COLUMN_ENTITY(rows, tag_to_add, 2);
    for(int i = 0; i < rows->count; i++){
        _ecs_remove(rows->world, rows->entities[i], tag_to_remove);
        // the following line doesn't work: Exception: EXC_BAD_ACCESS (code=1, address=0xf)
//        _ecs_add(rows->world, rows->entities[i], tag_to_add);
    }
}

void system_odds(ecs_rows_t *rows){
    printf("ods\n");
    ECS_COLUMN_ENTITY(rows, tag_to_remove, 1);
    ECS_COLUMN_ENTITY(rows, tag_to_add, 2);
    for(int i = 0; i < rows->count; i++){
        _ecs_add(rows->world, rows->entities[i], tag_to_add);
        _ecs_remove(rows->world, rows->entities[i], tag_to_remove);
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
    ecs_set_system_context(world, system_evens, tag_odds);

    ECS_SYSTEM(world, system_odds, EcsOnUpdate, tag_odds, .tag_evens);
    ecs_set_system_context(world, system_odds, tag_evens);


    int break_condition = 100;

    ECS_TYPE(world, Dummy, tag_evens);
    ecs_new_w_count(world, Dummy, 1);

    // The system crash here: Exception: EXC_BAD_ACCESS (code=1, address=0xe)
    // Stack Trace:
    /*
     ecs_vector_count
     ecs_type_merge
     ecs_merge_entity
     ecs_merge_commits
     ecs_stage_merge
     ecs_merge
     run_single_thread_stage
     ecs_process
     main.c
     */
    while(ecs_progress(world, 0) && break_condition--);

    ecs_fini(world);
}