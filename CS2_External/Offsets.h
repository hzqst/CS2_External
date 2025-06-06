#pragma once
#include <Windows.h>
#include "Utils/ProcessManager.hpp"

// From: https://github.com/a2x/cs2-dumper/blob/main/generated/client.dll.hpp
namespace Offset
{
	inline DWORD EntityList;
	inline DWORD Matrix;
	inline DWORD ViewAngle;
	inline DWORD LocalPlayerController;
	inline DWORD LocalPlayerPawn;
	inline DWORD ForceJump;
	inline DWORD GlobalVars;

	struct
	{
		DWORD Health = 0x344;
		DWORD TeamID = 0x3E3;
		DWORD IsAlive = 0x82C;
		DWORD PlayerPawn = 0x62C;
		DWORD iszPlayerName = 0x660;
	}Entity;

	struct
	{
		DWORD Pos = 0x1324;
		DWORD MaxHealth = 0x340;
		DWORD CurrentHealth = 0x344;
		DWORD GameSceneNode = 0x328;
		DWORD BoneArray = 0x1E0;
		DWORD angEyeAngles = 0x124C;
		DWORD vecLastClipCameraPos = 0x1384;
		DWORD pClippingWeapon = 0x13A0;
		DWORD iShotsFired = 0x23Fc;
		DWORD flFlashDuration = 0x140C;
		DWORD aimPunchAngle = 0x1584;
		DWORD aimPunchCache = 0x15A8;
		DWORD iIDEntIndex = 0x1458;
		DWORD iTeamNum = 0x3E3;
		DWORD CameraServices = 0x11E0;
		DWORD iFovStart = 0x214;
		DWORD fFlags = 0x3EC;
		DWORD bSpottedByMask = 0x11A8 + 0xC; // entitySpottedState + bSpottedByMask
	}Pawn;

	struct
	{
		DWORD RealTime = 0x00;
		DWORD FrameCount = 0x04;
		DWORD MaxClients = 0x10;
		DWORD IntervalPerTick = 0x14;
		DWORD CurrentTime = 0x2C;
		DWORD CurrentTime2 = 0x30;
		DWORD TickCount = 0x40;
		DWORD IntervalPerTick2 = 0x44;
		DWORD CurrentNetchan = 0x0048;
		DWORD CurrentMap = 0x0180;
		DWORD CurrentMapName = 0x0188;
	} GlobalVar;

	namespace Signatures
	{
		const std::string GlobalVars = "48 8B 05 ?? ?? ?? ?? 0F 57 C0 8B 48 48 33 C0";
		const std::string ViewMatrix = "48 8D 0D ?? ?? ?? ?? 48 C1 E0 06";
		const std::string ViewAngles = "48 8B 0D ?? ?? ?? ?? E9 ?? ?? ?? ?? CC CC CC CC 40 55";
		const std::string EntityList = "48 8B 0D ?? ?? ?? ?? 48 89 7C 24 ?? 8B FA C1";
		const std::string LocalPlayerController = "48 8B 1D ?? ?? ?? ?? 48 8B 6C 24 30 48 85 DB 74 ?? 8B BB F8 06 00 00";
		const std::string ForceJump = "48 8B 05 ?? ?? ?? ?? 48 8D 1D ?? ?? ?? ?? 48 89 45";
		const std::string LocalPlayerPawn = "48 8D 05 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC 48 83 EC ?? 8B 0D";
	}

	bool UpdateOffsets();
}
