#pragma once
#include <Windows.h>
#include "Utils/ProcessManager.hpp"
#include <client_dll.hpp>
#include <offsets.hpp>

// From: https://github.com/a2x/cs2-dumper/blob/main/output/client_dll.hpp
namespace Offset
{

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
			  sub_1807995A0(ClientInput, 0LL, &v65);
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

	const DWORD EntityList = cs2_dumper::offsets::client_dll::dwEntityList;
	const DWORD Matrix = cs2_dumper::offsets::client_dll::dwViewMatrix;
	const DWORD ViewAngle = cs2_dumper::offsets::client_dll::dwCSGOInput + Signatures::ClientInput_ViewAngle;
	const DWORD LocalPlayerController = cs2_dumper::offsets::client_dll::dwLocalPlayerController;
	const DWORD LocalPlayerPawn = cs2_dumper::offsets::client_dll::dwLocalPlayerPawn;
	inline DWORD ForceJump;
	const DWORD GlobalVars = cs2_dumper::offsets::client_dll::dwGlobalVars;

	struct
	{
		DWORD m_iHealth = cs2_dumper::schemas::client_dll::C_BaseEntity::m_iHealth;
		DWORD m_iMaxHealth = cs2_dumper::schemas::client_dll::C_BaseEntity::m_iMaxHealth;
		DWORD m_iTeamNum = cs2_dumper::schemas::client_dll::C_BaseEntity::m_iTeamNum;
		DWORD m_bPawnIsAlive = cs2_dumper::schemas::client_dll::CCSPlayerController::m_bPawnIsAlive;
		DWORD m_hPlayerPawn = cs2_dumper::schemas::client_dll::CCSPlayerController::m_hPlayerPawn;
		DWORD m_iszPlayerName = cs2_dumper::schemas::client_dll::CBasePlayerController::m_iszPlayerName;
		DWORD m_bGlow = cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Glow + cs2_dumper::schemas::client_dll::CGlowProperty::m_bGlowing;
		DWORD m_glowColorOverride = cs2_dumper::schemas::client_dll::C_BaseModelEntity::m_Glow + cs2_dumper::schemas::client_dll::CGlowProperty::m_glowColorOverride;
	}Entity;

	struct
	{
		DWORD m_vOldOrigin = cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_vOldOrigin;
		DWORD m_pGameSceneNode = cs2_dumper::schemas::client_dll::C_BaseEntity::m_pGameSceneNode;
		DWORD BoneArray = 0x210;
		DWORD m_angEyeAngles = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_angEyeAngles;
		DWORD m_vecLastClipCameraPos = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_vecLastClipCameraPos;
		DWORD m_pClippingWeapon = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_pClippingWeapon;
		DWORD m_iShotsFired = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired;
		DWORD m_flFlashDuration = cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_flFlashDuration;
		DWORD m_aimPunchAngle = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchAngle;
		DWORD m_aimPunchCache = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_aimPunchCache;
		DWORD m_iIDEntIndex = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iIDEntIndex;
		DWORD m_pCameraServices = cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pCameraServices;
		DWORD m_iFovStart = cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_iFOVStart;
		DWORD m_fFlags = cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags;
		DWORD m_bSpottedByMask = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask; // entitySpottedState + bSpottedByMask
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

	bool UpdateOffsets();
}
