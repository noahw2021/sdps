/*
main.c
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "inet/inet.h"
#include "services/matchmaking/matchmaking.h"
#include "services/matchmaking/endpoints/mm_endpoints.h"

BOOL 
WINAPI 
ControlHandler(
	DWORD ControlType
) {
	switch (ControlType) {
	case CTRL_C_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		if (InetCtx)
			InetCtx->ShutdownRequest = TRUE;
		break;
	}

	return FALSE;
}

BOOL APIENTRY WinMain(
	HINSTANCE Instance,
	HINSTANCE PrevInstance,
	LPSTR CommandLine,
	INT ShowMode
) {
	AllocConsole();
	SetConsoleTitleA("SDP Server");
	SetConsoleCtrlHandler(ControlHandler, TRUE);

	InRegisterApi("http://127.0.0.1:80/GameCoordinator/CreateSession?UserSecret=NUMBER", MmeCreateSession);

	InInit("http://sdp.trustup.care:80/");

	return TRUE;
}