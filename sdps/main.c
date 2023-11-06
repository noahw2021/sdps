/*
main.c
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include <windows.h>
#include "inet/inet.h"

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

	return TRUE;
}