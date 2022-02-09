#include "pch.h"
#include "opcode_mgr.h"

static HandlerResult TestOpcode(Context ctx)
{
	Log("Test Success");
	return HR_CONTINUE;
}


void OpcodeMgr::RegisterCommands()
{
	RegisterCommand("TEST_OPCODE", TestOpcode, "imgui");
}
