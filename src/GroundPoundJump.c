#include "mod.h"

#include "port/events/Events.h"
#include "game/mario.h"
#include "sm64.h"
static bool gHandlingGroundPoundLand;
static bool gGroundPoundJumpSparkles;

static bool JustLandedGroundPound(const struct MarioState* m) {
	return (m != NULL)
		&& (m->action == ACT_GROUND_POUND_LAND)
		&& (m->prevAction == ACT_GROUND_POUND)
		&& (m->actionTimer == 0);
}

static void OnGroundPoundLand(struct MarioState* m) {
	if (CVarGetInteger("gGroundPoundJump", 1) == 0) {
		return;
	}

	if ((m->input & INPUT_A_PRESSED) == 0) {
		return;
	}

	set_mario_action(m, ACT_TRIPLE_JUMP, 0);
	gGroundPoundJumpSparkles = true;
}

bool GroundPoundJump_HandleExecuteAction(PlayerExecuteAction* exec, struct MarioState* m) {
	if (gHandlingGroundPoundLand) {
		return false;
	}

	if (gGroundPoundJumpSparkles) {
		if (m == NULL || m->action != ACT_TRIPLE_JUMP || m->vel[1] < 0.0f) {
			gGroundPoundJumpSparkles = false;
		} else {
			m->particleFlags |= PARTICLE_SPARKLES;
		}
	}

	if (!JustLandedGroundPound(m)) {
		return false;
	}

	gHandlingGroundPoundLand = true;
	OnGroundPoundLand(m);
	gHandlingGroundPoundLand = false;

	if (m->action != ACT_TRIPLE_JUMP) {
		return false;
	}

	exec->Event.Cancelled = true;
	*(exec->result) = TRUE;
	return true;
}
