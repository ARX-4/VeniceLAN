#include "StdInc.h"

void VeniceLAN_Init()
{
	AllocConsole();
	freopen("CONOUT$", "w", stdout);

	BlazeLoop_Init();

	hook::set_base();

	HookFunction::RunAll();
}