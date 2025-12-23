#pragma once
#include "Game.h"
#include "View.hpp"
#include "Bone.h"
#include "Globals.hpp"

struct C_UTL_VECTOR
{
	DWORD64 Count = 0;
	DWORD64 Data = 0;
};

class PlayerController
{
public:
	DWORD64 Address = 0;
	int m_iTeamNum = 0;
	int m_iHealth = 0;
	int m_bPawnIsAlive = 0;
	DWORD Pawn = 0;
	std::string PlayerName;
public:
	bool GetTeamID();
	bool GetHealth();
	bool GetIsAlive();
	bool GetPlayerName();
	DWORD64 GetPlayerPawnAddress();
};

class PlayerPawn
{
public:
	enum class Flags
	{
		NONE,
		IN_AIR = 1 << 0
	};

	DWORD64 Address = 0;
	CBone BoneData;
	Vec2 ViewAngle;
	Vec3 m_vOldOrigin;
	Vec2 ScreenPos;
	Vec3 CameraPos;
	std::string WeaponName;
	DWORD ShotsFired;
	Vec2 AimPunchAngle;
	C_UTL_VECTOR AimPunchCache;
	int m_iHealth;
	int m_iTeamNum;
	int Fov;
	DWORD64 m_bSpottedByMask;
	int m_fFlags;
public:
	bool GetPos();
	bool GetViewAngle();
	bool GetCameraPos();
	bool GetWeaponName();
	bool GetShotsFired();
	bool GetAimPunchAngle();
	bool GetHealth();
	bool GetTeamID();
	bool GetFov();
	bool GetSpotted();
	bool GetFFlags();
	bool GetAimPunchCache();

	constexpr bool HasFlag(const Flags Flag) const noexcept {
		return m_fFlags & (int)Flag;
	}

	void SetGlow(bool b);
	void SetGlowColorOverride(DWORD color);
};

class CEntity
{
public:
	PlayerController Controller;
	PlayerPawn Pawn;
public:

	bool UpdateController(const DWORD64& PlayerControllerAddress);
	bool UpdatePawn(const DWORD64& PlayerPawnAddress);

	bool IsAlive();
	bool IsInScreen();
	CBone GetBone() const;
};