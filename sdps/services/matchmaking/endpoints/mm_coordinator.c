/*
mm_endpoints.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/
#include "mm_endpoints.h"

// http://127.0.0.1:80/GameCoordinator/StartMatchmaking?SessionId=NUMBER
/* Valid Response:
Success : True
* Error Response:
Success : False
*/
VOID WINAPI MmeStartMatchmaking(
	LPCSTR Url
) {
	LPSTR _SessionId = IniGetToken("SessionId");
	if (!_SessionId) {
		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	DWORD SessionId = strtoul(_SessionId, NULL, 10);
	IniFreeToken(_SessionId);
	if (SessionId > (MmClient->LobbyCount + 1) ||
		MmClient->Lobbies[SessionId].Invalid
		) {
		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	MmcUpdateGameServerInfo();

	PMATCHMAKING_LOBBY PartnerLobby = NULL;
	PMATCHMAKING_LOBBY Lobby = &MmClient->Lobbies[SessionId];
	Lobby->IsSearching = TRUE;

	for (int i = 0; i < MmClient->LobbyCount; i++) {
		PMATCHMAKING_LOBBY ThisLobby = &MmClient->Lobbies[i];
		if (ThisLobby->Invalid)
			continue;
		if (!ThisLobby->IsSearching)
			continue;
		
		PartnerLobby = NULL;
	}

	if (PartnerLobby) {
		for (int i = 0; i < MmClient->ServerCount; i++) {
			PMATCHMAKING_SERVER ThisServer = &MmClient->Servers[i];
			if (ThisServer->IsInUse)
				continue;

			ThisServer->IsInUse = TRUE;
			ThisServer->MmUsers = HeapAlloc(GetProcessHeap(), NULL, sizeof(PMATCHMAKING_USER) * 2);
			
			ThisServer->UserCount = 2;
			ThisServer->MmUsers[0] = Lobby->Users[0];
			ThisServer->MmUsers[1] = PartnerLobby->Users[0];

			PartnerLobby->ShouldDeliverGame = TRUE;
			PartnerLobby->PlannedServerId = ThisServer->ServerNumber;
			Lobby->ShouldDeliverGame = TRUE;
			Lobby->PlannedServerId = ThisServer->ServerNumber;
		}
	}

	PNETP_ENTRY Response = NpCreate();
	NpAddField(Response, "Success", "True");
	NpCalculate(Response);
	InRespondApi(200, "OK", Response->CalculatedString);
	NpDestroy(Response);
	return;
}

// http://127.0.0.1:80/GameCoordinator/StopMatchmaking?SessionId=NUMBER
/* Valid Response:
Success : True
 * Error Response:
Success : False
*/
VOID WINAPI MmeStopMatchmaking(
	LPCSTR Url
) {
	LPSTR _SessionId = IniGetToken("SessionId");
	if (!_SessionId) {
		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	DWORD SessionId = strtoul(_SessionId, NULL, 10);
	IniFreeToken(_SessionId);
	if (SessionId > (MmClient->LobbyCount + 1) ||
		MmClient->Lobbies[SessionId].Invalid
		) {
		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	PMATCHMAKING_LOBBY Lobby = &MmClient->Lobbies[SessionId];
	Lobby->IsSearching = FALSE;
	if (Lobby->HasOpponentAccepted) {
		PMATCHMAKING_LOBBY OpponentLobby = NULL;
		for (int i = 0; i < MmClient->LobbyCount; i++) {
			if (MmClient->Lobbies[i].PlannedServerId == Lobby->PlannedServerId)
				OpponentLobby = &MmClient->Lobbies[i];
		}

		if (OpponentLobby) {
			OpponentLobby->GameCancelled = TRUE;
			OpponentLobby->HasOpponentAccepted = FALSE;
			OpponentLobby->IsSearching = FALSE;
			OpponentLobby->PlannedServerId = -1;
		}
	}

	PNETP_ENTRY Response = NpCreate();
	NpAddField(Response, "Success", "True");
	NpCalculate(Response);
	InRespondApi(200, "OK", Response->CalculatedString);
	NpDestroy(Response);
	return;
}

// http://127.0.0.1:80/GameCoordinator/GetMatchmakingData?SessionId=NUMBER
/* Valid Response:
Success : True
PlayersSearching : <INT>
GameFound : <BOOL>
HasLocalAccepted : <BOOL>
HasOpponentAccepted : <BOOL>
GameStarted : <BOOL>
GameServerId : <INT>
 * Error Response:
Success : False
*/
VOID WINAPI MmeGetMatchmakingData(
	LPCSTR Url
) {
	LPSTR _SessionId = IniGetToken("SessionId");
	if (!_SessionId) {
		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	DWORD SessionId = strtoul(_SessionId, NULL, 10);
	IniFreeToken(_SessionId);
	if (SessionId > (MmClient->LobbyCount + 1) ||
		MmClient->Lobbies[SessionId].Invalid
		) {
		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	MmcUpdateGameServerInfo();
	PMATCHMAKING_LOBBY Lobby = &MmClient->Lobbies[SessionId];

	PNETP_ENTRY Response = NpCreate();
	NpAddField(Response, "Success", "True");

	PMATCHMAKING_LOBBY BestMatch = NULL;
	DWORD PlayerSearching = 0;
	for (int i = 0; i < MmClient->LobbyCount; i++) {
		PMATCHMAKING_LOBBY ThisLobby = &MmClient->Lobbies[i];
		if (ThisLobby->Invalid || ThisLobby->PlannedServerId != -1 || !ThisLobby->IsSearching)
			continue;

		PlayerSearching++;
		if (!BestMatch)
			BestMatch = ThisLobby;
	}

	if (BestMatch) {
		PMATCHMAKING_SERVER BestServer = NULL;
		for (int i = 0; i < MmClient->ServerCount; i++) {
			PMATCHMAKING_SERVER ThisServer = &MmClient->Servers[i];
			if (ThisServer->IsInUse)
				continue;
			if (!BestServer)
				BestServer = ThisServer;
		}

		if (BestServer) {
			BestServer->IsInUse = TRUE;
			BestServer->MmUsers = HeapAlloc(GetProcessHeap(), NULL, sizeof(PMATCHMAKING_USER) * 2);

			BestServer->UserCount = 2;
			BestServer->MmUsers[0] = Lobby->Users[0];
			BestServer->MmUsers[1] = BestMatch->Users[0];

			BestMatch->ShouldDeliverGame = TRUE;
			BestMatch->PlannedServerId = BestServer->ServerNumber;
			Lobby->ShouldDeliverGame = TRUE;
			Lobby->PlannedServerId = BestServer->ServerNumber;
		}
	}

	char PlayerSearchingStr[64];
	itoa(PlayerSearching, PlayerSearchingStr, 10);

	NpAddField(Response, "PlayersSearching", PlayerSearchingStr);
	NpAddField(Response, "GameFound", Lobby->ShouldDeliverGame ? "True" : "False");
	NpAddField(Response, "HasLocalAccepted", Lobby->HasLocalAccepted ? "True" : "False");
	NpAddField(Response, "HasOpponentAccepted", Lobby->HasOpponentAccepted ? "True" : "False");
	NpAddField(Response, "GameStarted",
		(Lobby->PlannedServerId > 0 && Lobby->HasLocalAccepted && Lobby->HasOpponentAccepted) ?
		"True" : "False"
	);
	NpAddField(Response, "GameCancelled", Lobby->GameCancelled ? "True" : "False");
	NpAddField(Response, "GameServerId", Lobby->PlannedServerId);

	Lobby->LastUserPing = GetTickCount64();

	NpCalculate(Response);
	InRespondApi(200, "OK", Response->CalculatedString);
	NpDestroy(Response);
}

// http://127.0.0.1:80/GameCoordinator/AcceptMatch?SessionId=NUMBER
/* Valid Response:
Success : True
 * Error Response:
Success : False
*/
VOID WINAPI MmeAcceptMatch(LPCSTR Url) {

}