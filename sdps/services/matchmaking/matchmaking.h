#pragma once
/*
matchmaking.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct _MATCHMAKING_USER {
	CHAR Username[64];
	DWORD64 LobbyId;
	BOOLEAN HasAccepted;
	DWORD ClientSecret;

}MATCHMAKING_USER, 
*PMATCHMAKING_USER;

typedef struct _MATCHMAKING_LOBBY {
	DWORD64 LobbyId;
	DWORD64 LastUserPing;

	DWORD UserCount;
	PMATCHMAKING_USER* Users;

	BOOLEAN IsSearching;
	BOOLEAN ShouldDeliverGame;
	BOOLEAN ReadyToAccept;
	BOOLEAN Invalid;
	BOOLEAN GameCancelled;

	INT PlannedServerId;
	BOOLEAN HasOpponentAccepted;
	BOOLEAN HasLocalAccepted;
}MATCHMAKING_LOBBY, 
*PMATCHMAKING_LOBBY;

typedef struct _MATCHMAKING_SERVER {
	INT ServerNumber;
	BOOLEAN IsInUse;

	DWORD UserCount;
	PMATCHMAKING_USER* MmUsers;
}MATCHMAKING_SERVER,
*PMATCHMAKING_SERVER;

typedef struct _MATCHMAKING_CLIENT {
	DWORD LobbyCount;
	DWORD UserCount;
	DWORD ServerCount;

	PMATCHMAKING_LOBBY Lobbies;
	PMATCHMAKING_USER Users;
	PMATCHMAKING_SERVER Servers;
}MATCHMAKING_CLIENT, 
*PMATCHMAKING_CLIENT;

extern PMATCHMAKING_CLIENT MmClient;

VOID MmInitSystems(VOID);
VOID MmShutdownSystem(VOID);

VOID MmcUpdateGameServerInfo(VOID);