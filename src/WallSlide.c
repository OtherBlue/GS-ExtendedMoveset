#include "mod.h"

#include "port/events/Events.h"
#include "game/mario.h"
#include "game/mario_step.h"
#include "sm64.h"
#include "mario_animation_ids.h"
#include "engine/math_util.h"

static bool gHandlingWallSlide;

static bool IsJumpingAction(u32 action) {
    switch (action) {
        case ACT_JUMP:
        case ACT_DOUBLE_JUMP:
        case ACT_TRIPLE_JUMP:
        case ACT_SPECIAL_TRIPLE_JUMP:
        case ACT_BACKFLIP:
        case ACT_STEEP_JUMP:
        case ACT_WALL_KICK_AIR:
        case ACT_SIDE_FLIP:
        case ACT_FREEFALL:
        case ACT_HOLD_JUMP:
        case ACT_HOLD_FREEFALL:
            return true;
        default:
            return false;
    }
}

static void UpdateWallSlide(struct MarioState* m) {
    if (m->actionTimer == 0) {
        m->wallKickTimer = 12;
        m->vel[1] = -4.0f;
        mario_set_forward_vel(m, 0.0f);
        if (m->wall != NULL) {
            m->faceAngle[1] = atan2s(m->wall->normal.z, m->wall->normal.x);
        }
    }

    if (m->input & INPUT_A_PRESSED) {
        set_mario_action(m, ACT_WALL_KICK_AIR, 0);
        return;
    }

    f32 targetVel = -4.0f - (m->actionTimer * 0.8f);
    if (targetVel < -24.0f) {
        targetVel = -24.0f;
    }
    m->vel[1] = targetVel;

    set_mario_animation(m, MARIO_ANIM_START_WALLKICK);

    switch (perform_air_step(m, 0)) {
        case AIR_STEP_LANDED:
            set_mario_action(m, ACT_FREEFALL_LAND, 0);
            break;

        case AIR_STEP_HIT_WALL:
            mario_set_forward_vel(m, 0.0f);
            break;
    }

    m->actionTimer++;
}

bool WallSlide_HandleExecuteAction(PlayerExecuteAction* exec, struct MarioState* m) {
    if (gHandlingWallSlide) {
        return false;
    }

    if (m == NULL || m->action != ACT_AIR_HIT_WALL || m->wall == NULL) {
        return false;
    }

    if (!IsJumpingAction(m->prevAction)) {
        return false;
    }

    exec->Event.Cancelled = true;
    *(exec->result) = false;

    gHandlingWallSlide = true;
    UpdateWallSlide(m);
    gHandlingWallSlide = false;
    return true;
}