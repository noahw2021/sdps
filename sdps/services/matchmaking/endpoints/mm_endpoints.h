#pragma once
/*
mm_endpoints.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "../matchmaking.h"
#include "../../../inet/inet.h"
#include "../../../netp/netp.h"

// http://127.0.0.1:80/GameCoordinator/CreateSession?UserSecret=NUMBER
/* Valid Response:
Success : True
SessionId : <NUMBER>
 * Error Response: 
Success : False
*/
VOID WINAPI MmeCreateSession(LPCSTR Url);

// http://127.0.0.1:80/GameCoordinator/ReportClientInformation?ClientSecret=NUMBER&Username=STRING
/* Valid Response:
Success : True
 * Error Response:
Success : False
*/
VOID WINAPI MmeReportClientInformation(
	LPCSTR Url);

// http://127.0.0.1:80/GameCoordinator/ValdiateMMToken?SessionId=NUMBER
/* Valid Response:
Success : True
Valid : <BOOL>
 * Error Response:
Success : False
*/
VOID WINAPI MmeValidateMMToken(
	LPCSTR Url);

// http://127.0.0.1:80/GameCoordinator/DeleteSession?SessionId=NUMBER
/* Valid Response:
Success : True
 * Error Response: 
Success : False
 */
VOID WINAPI MmeDeleteSession(LPCSTR Url);

// http://127.0.0.1:80/GameCoordinator/StartMatchmaking?SessionId=NUMBER
/* Valid Response:
Success : True
* Error Response:
Success : False
*/
VOID WINAPI MmeStartMatchmaking(
	LPCSTR Url);

// http://127.0.0.1:80/GameCoordinator/StopMatchmaking?SessionId=NUMBER
/* Valid Response:
Success : True
 * Error Response:
Success : False
*/
VOID WINAPI MmeStopMatchmaking(
	LPCSTR Url);

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
	LPCSTR Url);

// http://127.0.0.1:80/GameCoordinator/AcceptMatch?SessionId=NUMBER
/* Valid Response:
Success : True
 * Error Response:
Success : False
*/
VOID WINAPI MmeAcceptMatch(LPCSTR Url);