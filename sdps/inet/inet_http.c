/*
inet_http.c
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "inet.h"
#include <stdio.h>

LPSTR Backup;
VOID InListen(VOID) {
	Backup = NULL;

	DWORD Return = NULL;
	DWORD UrlCount = 0x00;
	HTTPAPI_VERSION HttpVersion = HTTPAPI_VERSION_1;

	Return = HttpInitialize(HttpVersion, HTTP_INITIALIZE_SERVER, NULL);
	if (Return != NO_ERROR) {
		LPSTR ErrorMessage = HeapAlloc(GetProcessHeap(), NULL, 512);
		sprintf(ErrorMessage, "[FATAL]: Failed to launch HTTP service, error = 0x%08lX.\n", 
			Return);

		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), ErrorMessage, strlen(ErrorMessage),
			NULL, NULL);
		HeapFree(GetProcessHeap(), NULL, ErrorMessage);
		return;
	}

	Return = HttpCreateHttpHandle(&InetCtx->RequestQueue, NULL);
	if (Return != NO_ERROR) {
		LPSTR ErrorMessage = HeapAlloc(GetProcessHeap(), NULL, 512);
		sprintf(ErrorMessage, "[FATAL]: Failed to create HTTP request, error = 0x%08lX.\n", 
			Return);

		WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), ErrorMessage, strlen(ErrorMessage),
			NULL, NULL);
		HeapFree(GetProcessHeap(), NULL, ErrorMessage);
		return;
	}

	for (int i = 0; i < InetCtx->EndPointCount; i++) {
		LPWSTR QualifiedString = HeapAlloc(GetProcessHeap(), NULL,
			(strlen(InetCtx->Endpoints[i].Url) * 2) + 64 + (strlen(InetCtx->UrlPrefix) * 2));
		MultiByteToWideChar(CP_OEMCP, NULL, InetCtx->Endpoints[i].Url, -1, QualifiedString,
			strlen(InetCtx->Endpoints[i].Url));

		Return = HttpAddUrl(InetCtx->RequestQueue, QualifiedString, NULL);
		if (Return != NO_ERROR) {
			LPSTR ErrorMessage = HeapAlloc(GetProcessHeap(), NULL, 512);
			sprintf(ErrorMessage, "[ERROR]: Failed to create endpoint, error = 0x%08lX.\n"
								  "[INFO]: Problem endpoint = '%s'.\n",
				Return, InetCtx->Endpoints[i].Url);

			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), ErrorMessage, strlen(ErrorMessage),
				NULL, NULL);
			HeapFree(GetProcessHeap(), NULL, ErrorMessage);
		}

		HeapFree(GetProcessHeap(), NULL, QualifiedString);
	}

	InRecieve(InetCtx->RequestQueue);
}

VOID InRecieve(HANDLE RequestQueue) {
	DWORD ThisRequestLength = sizeof(HTTP_REQUEST) + 4096;
	InetCtx->ThisRequest = HeapAlloc(GetProcessHeap(), NULL, ThisRequestLength);
	
	HTTP_REQUEST_ID RequestId;
	HTTP_SET_NULL_ID(&RequestId);

	while (TRUE) {
		if (InetCtx->ShutdownRequest)
			break;

		RtlSecureZeroMemory(InetCtx->ThisRequest, ThisRequestLength);

		DWORD BytesRead;
		DWORD Result = HttpReceiveHttpRequest(InetCtx->RequestQueue, RequestId, NULL,
			InetCtx->ThisRequest, ThisRequestLength, &BytesRead, NULL);

		if (Result == ERROR_MORE_DATA) {
			RequestId = InetCtx->ThisRequest->RequestId;
			ThisRequestLength = BytesRead;

			InetCtx->ThisRequest = HeapReAlloc(GetProcessHeap(), NULL, InetCtx->ThisRequest,
				ThisRequestLength);

			continue;
		} else if (Result == ERROR_CONNECTION_INVALID) {
			HTTP_SET_NULL_ID(&RequestId);
			continue;
		} else if (Result != NO_ERROR) {
			LPSTR ErrorMessage = HeapAlloc(GetProcessHeap(), NULL, 512);
			sprintf(ErrorMessage, "[ERROR]: Unknown HTTP error, error = 0x%08lX.\n",
				GetLastError());

			WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), ErrorMessage, strlen(ErrorMessage),
				NULL, NULL);
			HeapFree(GetProcessHeap(), NULL, ErrorMessage);
			continue;
		}

		for (int i = 0; i < InetCtx->EndPointCount; i++) {
			LPSTR AltQualUrl = HeapAlloc(GetProcessHeap(), NULL,
				strlen(InetCtx->Endpoints[i].Url) + 256);
			sprintf(AltQualUrl, "%s%s", InetCtx->UrlPrefix,
				strstr(InetCtx->Endpoints[i].Url, ":"));

			LPWSTR QualifiedStringW = HeapAlloc(GetProcessHeap(), NULL,
				(strlen(InetCtx->Endpoints[i].Url) * 2) + 64 + 
				(strlen(InetCtx->UrlPrefix) * 2));
			MultiByteToWideChar(CP_OEMCP, NULL, InetCtx->Endpoints[i].Url, 
				-1, QualifiedStringW, strlen(InetCtx->Endpoints[i].Url));

			LPWSTR AltQualifiedStringW = HeapAlloc(GetProcessHeap(), NULL,
				(strlen(AltQualUrl) * 2) + 1);
			MultiByteToWideChar(CP_OEMCP, NULL, InetCtx->Endpoints[i].Url,
				-1, AltQualifiedStringW, strlen(InetCtx->Endpoints[i].Url));

			if (wcsstr(InetCtx->ThisRequest->CookedUrl.pFullUrl, QualifiedStringW) ||
				wcsstr(InetCtx->ThisRequest->CookedUrl.pFullUrl, AltQualifiedStringW)
			) {
				if (InetCtx->ThisRequest->Verb != HttpVerbGET) {
					InRespondApi(405, "METHOD NOT ALLOWED", "You are not allowed to "
						"access this resource.\r\n");
				} else {
					InetCtx->Endpoints[i].FunctionPtr(InetCtx->ThisRequest->pRawUrl);
				}
			}

			HeapFree(GetProcessHeap(), NULL, QualifiedStringW);
			HeapFree(GetProcessHeap(), NULL, AltQualifiedStringW);
			HeapFree(GetProcessHeap(), NULL, AltQualUrl);
		}
	}

	InShutdown();
	return;
}

VOID InRespondApi(INT Code, LPCSTR Message, LPCSTR Data) {
	HTTP_RESPONSE Response;
	HTTP_DATA_CHUNK DataChunk;
	DWORD BytesSent;

	RtlSecureZeroMemory(&Response, sizeof(HTTP_RESPONSE));
	Response.StatusCode = Code;
	Response.pReason = Message;
	Response.ReasonLength = strlen(Data);

	Response.Headers.KnownHeaders[HttpHeaderContentType].pRawValue = "text/plain";
	Response.Headers.KnownHeaders[HttpHeaderContentType].RawValueLength = 10; // length

	if (Data) {
		RtlSecureZeroMemory(&DataChunk, sizeof(HTTP_DATA_CHUNK));
		
		DataChunk.DataChunkType = HttpDataChunkFromMemory;
		DataChunk.FromMemory.pBuffer = Data;
		DataChunk.FromMemory.BufferLength = strlen(Data);

		Response.EntityChunkCount = 1;
		Response.pEntityChunks = &DataChunk;
	}

	HttpSendHttpResponse(InetCtx->RequestQueue, InetCtx->ThisRequest->RequestId,
		NULL, &Response, NULL, &BytesSent, NULL, NULL, NULL, NULL);

	return;
}

LPSTR IniGetToken(LPCSTR Token) {
	if (!InetCtx->ThisRequest->CookedUrl.pQueryString)
		return NULL;
	if (!Backup)
		Backup = HeapAlloc(GetProcessHeap(), NULL, strlen(InetCtx->ThisRequest->CookedUrl.pQueryString) + 1);
	LPSTR Limited = Backup + 1;

	LPSTR Chunk = strtok(Limited, "&");
	while (Chunk) {
		LPSTR Chunk2 = strstr(Chunk, "=");
		if (!Chunk2)
			continue;

		// 64 is the max size of a for: &b=a
		if (!strncmp(Chunk, Token, (Chunk2 - Chunk)) && strlen(Chunk2 + 1) <= 64)
			return Chunk2 + 1;

		Chunk = strtok(NULL, "&");
	}
	
	return NULL;
}

VOID IniFreeToken(LPSTR Token) {
	return;
}