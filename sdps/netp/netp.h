#pragma once
/*
netp.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

typedef struct _NETP_FIELDPAIR {
	LPCSTR FieldName;
	LPCSTR FieldValue;
}NETP_FIELDPAIR, *PNETP_FIELDPAIR;

typedef struct _NETP_ENTRY {
	DWORD FieldPairCount;
	PNETP_FIELDPAIR FieldPairs;

	LPSTR CalculatedString;
}NETP_ENTRY, *PNETP_ENTRY;

PNETP_ENTRY NpCreate(VOID);
VOID        NpDestroy(PNETP_ENTRY Entry);

VOID NpAddField(PNETP_ENTRY Entry, LPCSTR Name, 
	LPCSTR Value);
VOID NpCalculate(PNETP_ENTRY Entry);