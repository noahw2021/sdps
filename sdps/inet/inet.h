#pragma once
/*
inet.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#define _WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <http.h>

typedef VOID(WINAPI* API_FUNCTION)(LPCSTR);

typedef struct _INET_ENDPOINT {
	API_FUNCTION FunctionPtr;
	LPSTR Url;
}INET_ENDPOINT, *PINET_ENDPOINT;

typedef struct _INETCTX {
	HANDLE RequestQueue;
	PHTTP_REQUEST ThisRequest;
	LPSTR UrlPrefix;
	BOOLEAN ShutdownRequest;

	DWORD EndPointCount;
	PINET_ENDPOINT Endpoints;
}INETCTX, *PINETCTX;
extern PINETCTX InetCtx;

VOID InInit(LPCSTR Url);
VOID InShutdown(VOID);

VOID InListen(VOID);
VOID InRecieve(HANDLE RequestQueue);
VOID InRegisterApi(LPCSTR Url, API_FUNCTION Ptr);
VOID InRespondApi(INT Code, LPCSTR Message, LPCSTR Data);

LPSTR IniGetToken(LPCSTR Token);
VOID IniFreeToken(LPSTR Token);