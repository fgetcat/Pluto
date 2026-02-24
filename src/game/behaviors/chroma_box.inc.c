#include "game/mario.h"
#include "game/mario_misc.h"
#include "game/obj_behaviors.h"
#include "game/object_helpers.h"
#include "engine/math_util.h"
#include "behavior_data.h"
#include "src/saturn/saturn.h"
#include "src/saturn/saturn_models.h"
#include "src/saturn/saturn_colors.h"

void bhv_chroma_box_init(void) {
    // Optionally set model or animation here
}

void bhv_chroma_box_loop(void) {
    if (gMarioStates[0].marioObj == NULL) return;
    if (!auto_chroma) return;

    struct MarioState *m = gMarioState;

    o->oPosX = m->pos[0];
    o->oPosY = m->floorHeight;
    o->oPosZ = m->pos[2];
}