/*
mm_init.c
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "matchmaking.h"
PMATCHMAKING_CLIENT MmClient;

VOID MmInitSystems(VOID) {
    MmClient = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY,
        sizeof(MATCHMAKING_CLIENT));

    MmcUpdateGameServerInfo();
    return;
}

VOID MmShutdownSystem(VOID) {
    if (!MmClient)
        return;

    if (MmClient->Lobbies) {
        for (int i = 0; i < MmClient->LobbyCount) {
            PMATCHMAKING_LOBBY ThisLobby = &MmClient->Lobbies[i];

            if (ThisLobby->Users)
                HeapFree(GetProcessHeap(), NULL, ThisLobby->Users);
        }

        HeapFree(GetProcessHeap(), NULL, MmClient->Lobbies); 
    }

    if (MmClient->Users)
        HeapFree(GetProcessHeap(), NULL, MmClient->Users);

    if (MmClient->Servers) {
        for (int i = 0; i < MmClient->ServerCount; i++) {
            PMATCHMAKING_SERVER ThisServer = &MmClient->Servers[i];

            if (ThisServer->MmUsers)
                HeapFree(GetProcessHeap(), NULL, ThisServer->MmUsers);
        }
        HeapFree(GetProcessHeap(), NULL, MmClient->Servers);
    }

    HeapFree(GetProcessHeap(), NULL, MmClient);
    return;
}