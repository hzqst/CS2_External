#pragma once
#include <Windows.h>
#include "Utils/ProcessManager.hpp"

// From: https://github.com/a2x/cs2-dumper/blob/main/output/client_dll.hpp
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
		DWORD m_iHealth = 0x34C;
		DWORD m_iTeamNum = 0x3EB;
		DWORD m_bPawnIsAlive = 0x904;
		DWORD m_hPlayerPawn = 0x8FC;
		DWORD m_iszPlayerName = 0x6E8;
	}Entity;

	struct
	{
		DWORD m_vOldOrigin = 0x15B0;
		DWORD m_iMaxHealth = 0x348;
		DWORD m_iHealth = 0x34C;
		DWORD m_pGameSceneNode = 0x330;
		DWORD BoneArray = 0x1F0;
		DWORD m_angEyeAngles = 0x16A0;
		DWORD m_vecLastClipCameraPos = 0x1604;
		DWORD m_pClippingWeapon = 0x1620;
		DWORD m_iShotsFired = 0x28C4;
		DWORD m_flFlashDuration = 0x167C;
		DWORD m_aimPunchAngle = 0x185C;
		DWORD m_aimPunchCache = 0x1880;
		DWORD m_iIDEntIndex = 0x1734;
		DWORD m_iTeamNum = 0x3EB;
		DWORD m_pCameraServices = 0x1438;
		DWORD m_iFovStart = 0x28C;
		DWORD m_fFlags = 0x3F8;
		DWORD m_bSpottedByMask = 0x1180 + 0xC; // entitySpottedState + bSpottedByMask
	}Pawn;

	struct
	{
		DWORD RealTime = 0x00;
		DWORD FrameCount = 0x04;
		DWORD MaxClients = 0x10;
		DWORD IntervalPerTick = 0x14;
		DWORD CurrentTime = 0x30;
		DWORD TickCount = 0x44;
		DWORD IntervalPerTick2 = 0x44;
		DWORD CurrentMapName = 0x58;
	} GlobalVar;

	namespace Signatures
	{
		const std::string GlobalVars = "48 89 15 ?? ?? ?? ?? 48 89 42";
		const std::string ViewMatrix = "48 8D 0D ?? ?? ?? ?? 48 C1 E0 06";

		/*
			}
			if ( !strcmp("bot_takeover", v4) )
			{
			  v61 = sub_1807AA730(a2, "p");
			  v65 = *(float *)&v61;
			  v62 = sub_1807AA730(a2, "y");
			  v66 = *(float *)&v62;
			  v63 = sub_1807AA730(a2, "r");
			  LODWORD(v67) = LODWORD(v63);
			  sub_1807995A0(ViewAngles, 0LL, &v65);
			}
		*/

		const std::string ClientInput = "48 8B 0D ?? ?? ?? ?? 4C 8D 44 24 40 33 D2 F3 0F";
		const size_t ClientInput_ViewAngle = 0x2CC;

		const std::string EntityList = "48 8B 0D ?? ?? ?? ?? 48 89 7C 24 ?? 8B FA C1";
		const std::string LocalPlayerController = "48 8B 15 ?? ?? ?? ?? 48 85 D2 74 ?? 8B 92 B4 06 00 00";
		const std::string ForceJump = "48 8B 05 ?? ?? ?? ?? 48 8D 1D ?? ?? ?? ?? 48 89 45";
		const std::string Prediction = "48 8D 05 ?? ?? ?? ?? C3 CC CC CC CC CC CC CC CC 40 53 56 41 54";
		const size_t Prediction_LocalPlayerPawn = 0xD0;
	}

	bool UpdateOffsets();
}
