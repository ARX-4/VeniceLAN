#include "StdInc.h"
#include "BlazeLoop.h"

static HookFunction hookFunction([]
{
	// don't connect securely to the redirector server
	//hook::put<char>(hook::pattern("6A 01 66 0F D6 44 24 ? 66 0F EF C0").count(1).get(0).get<void>(1), 0);

	// hook ws2_32 imports
	hook::iat("ws2_32.dll", BlazeLoop_CloseSocket, 3);
	hook::iat("ws2_32.dll", BlazeLoop_Connect, 4);
	hook::iat("ws2_32.dll", BlazeLoop_Recv, 16);
	hook::iat("ws2_32.dll", BlazeLoop_RecvFrom, 17);
	hook::iat("ws2_32.dll", BlazeLoop_Select, 18);
	hook::iat("ws2_32.dll", BlazeLoop_Send, 19);
	hook::iat("ws2_32.dll", BlazeLoop_SendTo, 20);
	hook::iat("ws2_32.dll", BlazeLoop_GetHostByName, 52);
});