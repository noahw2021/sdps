/*
mm_sessions.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "mm_endpoints.h"

// http://127.0.0.1:80/GameCoordinator/CreateSession?UserSecret=NUMBER
/* Valid Response:
Success : True
SessionId : <NUMBER>
 * Error Response:
Success : False
*/
VOID WINAPI MmeCreateSession(LPCSTR Url) {
	LPSTR _UserSecret = IniGetToken("UserSecret");
	if (!_UserSecret) {
		PNETP_ENTRY NetReturn = NpCreate();
		NpAddField(NetReturn, "Success", "False");
		NpCalculate(NetReturn);

		InRespondApi(200, "OK", NetReturn->CalculatedString);
		NpDestroy(NetReturn);

		return;
	}

	MATCHMAKING_LOBBY Session;
	RtlSecureZeroMemory(&Session, sizeof(MATCHMAKING_LOBBY));

	MATCHMAKING_USER User;
	RtlSecureZeroMemory(&User, sizeof(MATCHMAKING_USER));
	User.ClientSecret = strtoull(_UserSecret, NULL, 10);
	User.LobbyId = MmClient->LobbyCount;
	IniFreeToken(_UserSecret);

	Session.UserCount = 1;
	Session.Users = HeapAlloc(GetProcessHeap(), NULL, sizeof(PMATCHMAKING_USER));
	Session.LobbyId = MmClient->LobbyCount;

	if (!MmClient->Users) {
		MmClient->Users = HeapAlloc(GetProcessHeap(), NULL, sizeof(MATCHMAKING_USER));
	} else {
		MmClient->Users = HeapReAlloc(GetProcessHeap(), NULL, MmClient->Users,
			sizeof(MATCHMAKING_USER) * (MmClient->UserCount + 1));
	}

	if (!MmClient->Lobbies) {
		MmClient->Lobbies = HeapAlloc(GetProcessHeap(), NULL, sizeof(MATCHMAKING_LOBBY));
	} else {
		MmClient->Lobbies = HeapReAlloc(GetProcessHeap(), NULL, MmClient->Lobbies,
			sizeof(MATCHMAKING_LOBBY) * (MmClient->UserCount + 1));
	}

	PMATCHMAKING_LOBBY ThisLobby = &MmClient->Lobbies[MmClient->LobbyCount++];
	PMATCHMAKING_USER ThisUser = &MmClient->Users[MmClient->UserCount++];

	Session.Users[Session.UserCount - 1] = ThisUser;

	memcpy(ThisLobby, &Session, sizeof(MATCHMAKING_LOBBY));
	memcpy(ThisUser, &User, sizeof(MATCHMAKING_USER));

	char LobbyIdStr[64];
	itoa(ThisLobby->LobbyId, LobbyIdStr, 10);

	PNETP_ENTRY Response = NpCreate();
	NpAddField(Response, "Success", "True");
	NpAddField(Response, "SessionId", LobbyIdStr);
	NpCalculate(Response);
	InRespondApi(200, "OK", Response->CalculatedString);
	NpDestroy(Response);
	
	return;
}

// http://127.0.0.1:80/GameCoordinator/ReportClientInformation?ClientSecret=NUMBER&Username=STRING
/* Valid Response:
Success : True
 * Error Response:
Success : False
*/
VOID WINAPI MmeReportClientInformation(LPCSTR Url) {
	LPSTR _ClientSecret = IniGetToken("ClientSecret");
	LPSTR _Username = IniGetToken("Username");

	if (!_ClientSecret || !_Username) {
		if (_ClientSecret)
			IniFreeToken(_ClientSecret);
		if (_Username)
			IniFreeToken(_Username);

		PNETP_ENTRY Response = NpCreate();
		NpAddField(Response, "Success", "False");
		NpCalculate(Response);
		InRespondApi(200, "OK", Response->CalculatedString);
		NpDestroy(Response);
		return;
	}

	DWORD ClientSecret = strtoull(_ClientSecret, NULL, 10);
	IniFreeToken(_ClientSecret);

	BOOLEAN Found = FALSE;
	for (int i = 0; i < MmClient->UserCount; i++) {
		PMATCHMAKING_USER ThisUser = &MmClient->Users[i];

		if (ThisUser->ClientSecret == ClientSecret) {
			strcpy_s(ThisUser->Username, 64, _Username);
			Found = TRUE;
			break;
		}
	}
	IniFreeToken(_Username);

	PNETP_ENTRY Response = NpCreate();
	NpAddField(Response, "Success", Found ? "True" : "False");
	NpCalculate(Response);
	InRespondApi(200, "OK", Response->CalculatedString);
	NpDestroy(Response);

	return;
}

// http://127.0.0.1:80/GameCoordinator/ValdiateMMToken?SessionId=NUMBER
/* Valid Response:
Success : True
Valid : <BOOL>
 * Error Response:
Success : False
*/
VOID WINAPI MmeValidateMMToken(
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

	PNETP_ENTRY Response = NpCreate();
	NpAddField(Response, "Success", "True");
	if (SessionId > (MmClient->LobbyCount + 1) ||
		MmClient->Lobbies[SessionId].Invalid
	) {
		NpAddField(Response, "Valid", "False");
	} else {
		NpAddField(Response, "Valid", "True");
	}

	NpCalculate(Response);
	InRespondApi(200, "OK", Response->CalculatedString);
	NpDestroy(Response);
	return;
}

// http://127.0.0.1:80/GameCoordinator/DeleteSession?SessionId=NUMBER
/* Valid Response:
Success : True
 * Error Response:
Success : False
 */
VOID WINAPI MmeDeleteSession(LPCSTR Url) {
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
	Lobby->Invalid = TRUE;
	Lobby->UserCount = 0;
	Lobby->Users = HeapFree(GetProcessHeap(), NULL, Lobby->Users);

	return;
}