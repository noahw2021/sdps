#pragma once
/*
game.h
OSU Software Design Project -- Game Server / Game Coordinator
sdps

(c) Noah Wooten 2023, All Rights Reserved
OSU Software Design Project Game Server
*/

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#define GAME_OBJECTIVE_FLAG 0x01
#define GAME_OBJECTIVE_NONE 0x02
#define GAME_OBJECTIVE_RES1 0x03
#define GAME_OBJECTIVE_RES2 0x04
#define GAME_OBJECTIVE_RES3 0x05

#define GAME_ENTITY_PLAYER     0x06
#define GAME_ENTITY_FLAG       0x07
#define GAME_ENTITY_WALL       0x08
#define GAME_ENTITY_GOAL       0x09
#define GAME_ENTITY_PROJECTILE 0x0A

#define GAME_SENDER_SERVER 0x10
#define GAME_SENDER_CLIENT 0x11

typedef struct _GAME_MESSAGE {
    LPSTR Message;
    DWORD Sender;
}GAME_MESSAGE, *PGAME_MESSAGE;

typedef struct _GAME_RULES {
    DWORD MaxPlayerCount;
    DWORD ObjectiveType;

    DWORD MaxSanePosition;
    DWORD MapWidth;
    DWORD MapHeight;

    BOOLEAN InFreezeTime;
    DWORD FreezeTimeOver;
    BOOLEAN IsGameReady;
    DWORD GameScore[2];
}GAME_RULES, *PGAME_RULES;

typedef struct _GAME_ENTITY {
    DWORD EntityType;
    DWORD PositionX, PositionY;
    DWORD AimPosX, AimPosY;
    DWORD EntityTextureId;
    DWORD EntityHealth;
    DWORD EntityMaxHealth;
    BOOLEAN EntityHasHealth;
    BOOLEAN HasWeapon;
    BOOLEAN EntityComputeClipping;

    struct {
        BOOLEAN IsFiring;
        DWORD WeaponId;
        DWORD NextFireTick;
        DWORD AmmoCount;
        DWORD ReserveMagazines;
    }WeaponInfo;

    struct {
        BOOLEAN EntityOccludes;
        BOOLEAN EntityCanNotPass;
    }Clipping;


    struct {
        CHAR PlayerName[64];
        BOOLEAN HasFlag;
    }PlayerData;

    struct {
        BOOLEAN IsCarried;
        DWORD CarryingPlayer;
    }ObjectiveData;
}GAME_ENTITY, *PGAME_ENTITY

typedef struct _GAME_SERVER {
    DWORD TickRate;
    DWORD TickBase;
    DWORD ClientSecrets[2];
    DWORD GameExpirationTick;

    DWORD EntityCount;
    PGAME_ENTITY Entities;

    DWORD GameMessageCount;
    PGAME_MESSAGE Messages;

    PGAME_RULES GameRules;
}GAME_SERVER, *PGAME_SERVER;

VOID GsInit(VOID);
VOID GsShutdown(VOID);

VOID GsCreateServers(INT MaxCount);