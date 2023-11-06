/*
inet_init.c
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "inet.h"
PINETCTX InetCtx;

VOID InInit(LPCSTR Url) {
	InetCtx = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(INETCTX));
	if (!InetCtx)
		return;

	InetCtx->UrlPrefix = HeapAlloc(GetProcessHeap(), NULL, strlen(Url) + 1);
	strcpy(InetCtx->UrlPrefix, Url);
	
	return;
}

VOID InShutdown(VOID) {
	if (!InetCtx)
		return;

	if (InetCtx->UrlPrefix)
		HeapFree(GetProcessHeap(), NULL, InetCtx->UrlPrefix);
	
	if (InetCtx->Endpoints) {
		for (int i = 0; i < InetCtx->EndPointCount; i++)
			HeapFree(GetProcessHeap(), NULL, InetCtx->Endpoints[i].Url);
		HeapFree(GetProcessHeap(), NULL, InetCtx->Endpoints);
	}

	if (InetCtx->RequestQueue != INVALID_HANDLE_VALUE)
		HttpShutdownRequestQueue(InetCtx->RequestQueue);

	HeapFree(GetProcessHeap(), NULL, InetCtx);
	return;
}

VOID InRegisterApi(LPCSTR Url, API_FUNCTION Ptr) {
	if (!InetCtx)
		return;
	if (!strstr(Url, ":")) // port specifier
		return;

	if (!InetCtx->Endpoints) {
		InetCtx->Endpoints = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
			sizeof(INET_ENDPOINT));
	} else {
		InetCtx->Endpoints = HeapReAlloc(GetProcessHeap(), NULL, InetCtx->Endpoints,
			sizeof(INET_ENDPOINT) * (InetCtx->EndPointCount + 1));
	}

	PINET_ENDPOINT NewPoint = &InetCtx->Endpoints[InetCtx->EndPointCount];
	InetCtx->EndPointCount++;

	NewPoint->FunctionPtr = Ptr;
	NewPoint->Url = HeapAlloc(GetProcessHeap(), NULL, strlen(Url) + 1);
	strcpy(NewPoint->Url, Url);

	return;
}