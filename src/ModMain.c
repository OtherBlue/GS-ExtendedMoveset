#include "mod.h"

#include "port/events/Events.h"
#include "game/level_update.h"
#include "port/api/ui.h"
#include "GroundPoundJump.h"
#include "WallSlide.h"

static ListenerID gExecuteActionListenerID;
static bool gHandlingExecuteAction;

static void SetupUI(void) {
	C_WidgetConfig groundPound = {0};
	groundPound.type = C_WIDGET_CVAR_CHECKBOX;
	groundPound.cvar = "gGroundPoundJump";
	groundPound.opts.checkbox.tooltip = "Gives Mario the ability to do a super jump out of a ground pound.";
	groundPound.opts.checkbox.default_val = true;

	C_WidgetConfig wallSlide = {0};
	wallSlide.type = C_WIDGET_CVAR_CHECKBOX;
	wallSlide.cvar = "gWallSlide";
	wallSlide.opts.checkbox.tooltip = "Allow Mario to slide down walls.";
	wallSlide.opts.checkbox.default_val = true;

	C_AddSidebarEntry("Extended Moveset", 1);
	C_AddWidget("Extended Moveset", 1, "Ground Pound Jump", &groundPound);
	C_AddWidget("Extended Moveset", 1, "Wall Slide", &wallSlide);
}

static void OnPlayerExecuteAction(IEvent* event) {
	if (gHandlingExecuteAction) {
		return;
	}

	if (gMarioState == NULL) {
		return;
	}

	PlayerExecuteAction* exec = (PlayerExecuteAction*)event;

	gHandlingExecuteAction = true;
	if (CVarGetInteger("gGroundPoundJump", 1) != 0) {
		GroundPoundJump_HandleExecuteAction(exec, gMarioState);
	}
	if (CVarGetInteger("gWallSlide", 1) != 0) {
		WallSlide_HandleExecuteAction(exec, gMarioState);
	}
	gHandlingExecuteAction = false;
}

MOD_INIT() {
	SetupUI();
	gExecuteActionListenerID = REGISTER_LISTENER(PlayerExecuteAction, EVENT_PRIORITY_HIGH, OnPlayerExecuteAction);
}

MOD_EXIT() {
	C_RemoveSidebarEntry("Extended Moveset");
	UNREGISTER_LISTENER(PlayerExecuteAction, gExecuteActionListenerID);
}
