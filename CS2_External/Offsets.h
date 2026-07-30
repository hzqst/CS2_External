#pragma once
#include <Windows.h>
#include "Utils/ProcessManager.hpp"
#include <client_dll.hpp>
#include <offsets.hpp>

// From: https://github.com/a2x/cs2-dumper/blob/main/output/client_dll.hpp
namespace Offset
{
	const size_t ClientInput_ViewAngle = 0x688;// CS2_VibeSignatures: client.dll -> CClientInput_m_viewangles

	const DWORD EntityList = cs2_dumper::offsets::client_dll::dwEntityList;
	const DWORD Matrix = cs2_dumper::offsets::client_dll::dwViewMatrix;
	const DWORD ViewAngle = cs2_dumper::offsets::client_dll::dwCSGOInput + ClientInput_ViewAngle;
	const DWORD LocalPlayerController = cs2_dumper::offsets::client_dll::dwLocalPlayerController;
	const DWORD LocalPlayerPawn = cs2_dumper::offsets::client_dll::dwLocalPlayerPawn;
	inline DWORD ForceJump;
	const DWORD GlobalVars = cs2_dumper::offsets::client_dll::dwGlobalVars;
	// Pointer to the live `sensitivity` convar object; the float lives at +0x58.
	const DWORD Sensitivity = cs2_dumper::offsets::client_dll::dwSensitivity;
	const DWORD Sensitivity_Value = cs2_dumper::offsets::client_dll::dwSensitivity_sensitivity;

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
		// CSkeletonInstance derives from CGameSceneNode; the bone-matrix cache sits at
		// m_modelState + 0x80 (the matrix array inside CModelState). Hardcoding 0x210
		// breaks whenever the game shifts m_modelState — pull it from cs2-dumper.
		DWORD BoneArray = 0x1C0; // CS2_VibeSignatures: client.dll -> CSkeletonInstance_m_modelState_m_simulationState
		DWORD m_angEyeAngles = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_angEyeAngles;
		// m_vecLastClipCameraPos was removed from the schema; the local camera
		// origin is now sourced from the CViewRender singleton (see CView).
		// m_pClippingWeapon was removed from the schema; the active weapon is
		// now resolved via m_pWeaponServices -> m_hActiveWeapon (see Weapon).
		DWORD m_pWeaponServices = cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pWeaponServices;
		DWORD m_iShotsFired = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iShotsFired;
		DWORD m_flFlashDuration = cs2_dumper::schemas::client_dll::C_CSPlayerPawnBase::m_flFlashDuration;
		DWORD m_iIDEntIndex = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_iIDEntIndex;
		DWORD m_pCameraServices = cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_pCameraServices;
		DWORD m_flFOVSensitivityAdjust = cs2_dumper::schemas::client_dll::C_BasePlayerPawn::m_flFOVSensitivityAdjust;
		DWORD m_iFovStart = cs2_dumper::schemas::client_dll::CCSPlayerBase_CameraServices::m_iFOVStart;
		DWORD m_fFlags = cs2_dumper::schemas::client_dll::C_BaseEntity::m_fFlags;
		DWORD m_bSpotted = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpotted;
		DWORD m_bSpottedByMask = cs2_dumper::schemas::client_dll::C_CSPlayerPawn::m_entitySpottedState + cs2_dumper::schemas::client_dll::EntitySpottedState_t::m_bSpottedByMask; // entitySpottedState + bSpottedByMask
	}Pawn;

	struct
	{
		DWORD m_hActiveWeapon = cs2_dumper::schemas::client_dll::CPlayer_WeaponServices::m_hActiveWeapon;
		// vdata lives at C_BaseEntity::m_nSubclassID + 0x8 (CCSWeaponBaseVData*)
		DWORD m_nSubclassID = cs2_dumper::schemas::client_dll::C_BaseEntity::m_nSubclassID;
		DWORD m_szName = cs2_dumper::schemas::client_dll::CCSWeaponBaseVData::m_szName;
	}Weapon;

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
