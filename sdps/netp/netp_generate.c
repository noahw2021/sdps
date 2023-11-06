/*
netp_generate.c
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "netp.h"

#pragma warning(disable: 6001 6308 28182) // ?? 

PNETP_ENTRY NpCreate(VOID) {
	PNETP_ENTRY Entry = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(NETP_ENTRY));
	return Entry;
}

VOID NpDestroy(PNETP_ENTRY Entry) {
	if (!Entry)
		return;

	if (Entry->CalculatedString)
		HeapFree(GetProcessHeap(), NULL, Entry->CalculatedString);

	if (Entry->FieldPairs) {
		for (int i = 0; i < Entry->FieldPairCount; i++) {
			if (Entry->FieldPairs[i].FieldName)
				HeapFree(GetProcessHeap(), NULL, Entry->FieldPairs[i].FieldName);
			if (Entry->FieldPairs[i].FieldValue)
				HeapFree(GetProcessHeap(), NULL, Entry->FieldPairs[i].FieldValue);
		}

		HeapFree(GetProcessHeap(), NULL, Entry->FieldPairs);
	}

	HeapFree(GetProcessHeap(), NULL, Entry);
	return;
}

VOID NpAddField(PNETP_ENTRY Entry, LPCSTR Name, LPCSTR Value) {
	NETP_FIELDPAIR NewFp;

	NewFp.FieldName = HeapAlloc(GetProcessHeap(), NULL, strlen(Name) + 1);
	if (!NewFp.FieldName)
		return;

	NewFp.FieldValue = HeapAlloc(GetProcessHeap(), NULL, strlen(Value) + 1);
	if (!NewFp.FieldValue) {
		HeapFree(GetProcessHeap(), NULL, NewFp.FieldName);
		return;
	}

	strcpy(NewFp.FieldName, Name);
	strcpy(NewFp.FieldValue, Value);

	if (!Entry->FieldPairs) {
		Entry->FieldPairs = HeapAlloc(GetProcessHeap(), NULL, sizeof(NETP_FIELDPAIR));
	} else {
		Entry->FieldPairs = HeapReAlloc(GetProcessHeap(), NULL, Entry->FieldPairs,
			sizeof(NETP_FIELDPAIR) * (Entry->FieldPairCount + 1));
	}

	PNETP_FIELDPAIR NextPair = &Entry->FieldPairs[Entry->FieldPairCount++];
	memcpy(NextPair, &NewFp, sizeof(NETP_FIELDPAIR));

	return;
}

VOID NpCalculate(PNETP_ENTRY Entry) {
	DWORD CalculatedStringBuffer = 512;
	DWORD CurrentPosition = 0;

	LPSTR StringBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, 
		CalculatedStringBuffer);
	if (!StringBuffer)
		return;

	for (int i = 0; i < Entry->FieldPairCount; i++) {
		PNETP_FIELDPAIR ThisPair = &Entry->FieldPairs[i];

		if ((strlen(ThisPair->FieldName) + strlen(ThisPair->FieldValue) + 10) 
			> (CalculatedStringBuffer - CurrentPosition)
		) {
			CalculatedStringBuffer += 1024 + strlen(ThisPair);
			StringBuffer = HeapReAlloc(GetProcessHeap(), NULL, StringBuffer, 
				CalculatedStringBuffer);
		
			if (!StringBuffer)
				return;
		}

		strcat(StringBuffer, ThisPair->FieldName);
		strcat(StringBuffer, " : ");
		strcat(StringBuffer, ThisPair->FieldValue);
		strcat(StringBuffer, "\n");
	}

	if (Entry->CalculatedString)
		HeapFree(GetProcessHeap(), NULL, Entry->CalculatedString);

	Entry->CalculatedString = StringBuffer;
	return;
}