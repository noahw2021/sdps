#pragma once
/*
sg_endpoints.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#include "../game.h"

// http://127.0.0.1:80/GameServer/ConnectServer?ClientSecret=INT&ServerId=INT
/* Success Response:
Success : True
 * Error Response:
Success : False
*/
VOID SgeConnectServer(LPCSTR Url);

// http://127.0.0.1:80/GameServer/DisconnectServer?ClientSecret=INT
/* Success Reponse:
Success : True
 * Error Response:
Success : False
*/
VOID SgeDisconnectServer(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetGameRules?ClientSecret=INT
/* Success Response:
Success : True
MaxPlayerCount : <INT>
ObjectiveType : <INT>
MaxSanePosition : <INT>
MapWidth : <INT>
MapHeight : <INT>
IsGameReady : <BOOL>
InFreezeTime : <BOOL>
GameScore0 : <INT>
GameScore1 : <INT>
 * Error Response:
Success : False
*/
VOID SgeGetGameRules(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityCount?ClientSecret=INT
/* Success Response:
Success : True
EntityCount : <INT>
 * Error Response:
Success : False
*/
VOID SgeGetEntityCount(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityInformationBase?ClientSecret=INT&EntityId=INT
/* Success Response:
Success : True
EntityType : <INT>
PositionX : <INT>
PositionY : <INT>
AimPosX : <INT>
AimPosY: <INT>
EntityTextureId : <INT>
EntityHealth : <INT>
EntityMaxHealth : <INT>
EntityHasHealth : <BOOL>
HasWeapon : <BOOL>
EntityComputeClipping : <BOOL>
 * Error Response:
Success : False
*/
VOID SgeEntityInformationBase(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityInformationPlayer?ClientSecret=INT&EntityId=INT
/* Success Response:
Success : True 

 * Error Response:
Success : False
*/
VOID SgeGetEntityInformationPlayer(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityInformationFlag?ClientSecret=INT&EntityId=INT
VOID SgeGetEntityInformationFlag(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityInformationWall?ClientSecret=INT&EntityId=INT
VOID SgeGetEntityInformationWall(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityInformationGoal?ClientSecret=INT&EntityId=INT
VOID SgeGetEntityInformationGoal(LPCSTR Url);

// http://127.0.0.1:80/GameServer/GetEntityInformationProjectile?ClientSecret=INT&EntityId=INT
VOID SgeGetEntityInformationProjectile(LPCSTR Url);

// http://127.0.0.1:80/GameServer/UpdateMove?ClientSecret=INT&DeltaX=INT&DeltaY=INT&AimPositionX=INT&AimPositionY=INT
VOID SgeUpdateMove(LPCSTR Url);

// http://127.0.0.1:80/GameServer/UpdateAction?ClientSecret=INT&ActionType=INT&ActionArgument=INT
VOID SgeUpdateAction(LPCSTR Url);