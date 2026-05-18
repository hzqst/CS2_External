#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Game.h"
#include "Entity.h"
#include "MenuConfig.hpp"
#include <iostream>

namespace AimControl
{
	inline int HotKey = VK_LBUTTON;
	inline float AimFov = 5;
	inline float Smooth = 0.7;
	inline std::vector<int> HotKeyList{VK_LBUTTON, VK_LMENU, VK_RBUTTON, VK_XBUTTON1, VK_XBUTTON2, VK_CAPITAL, VK_LSHIFT, VK_LCONTROL};// added new button VK_LBUTTON

	inline void SetHotKey(int Index)
	{
		HotKey = HotKeyList.at(Index);
	}

	inline void AimBot(const CEntity& Local, Vec3 LocalPos, Vec3 AimPos)
	{
		float Yaw, Pitch;
		float Distance, Norm;
		Vec3 OppPos;

		OppPos = AimPos - LocalPos;

		Distance = sqrt(pow(OppPos.x, 2) + pow(OppPos.y, 2));

		Yaw = atan2f(OppPos.y, OppPos.x) * 57.295779513 - Local.Pawn.ViewAngle.y;
		Pitch = -atan(OppPos.z / Distance) * 57.295779513 - Local.Pawn.ViewAngle.x;
		Norm = sqrt(pow(Yaw, 2) + pow(Pitch, 2));
		if (Norm > AimFov)
			return;

		if (MenuConfig::LegitInput)
		{
			// Normalize yaw delta into [-180, 180] so we never pick the long way around.
			while (Yaw > 180.f) Yaw -= 360.f;
			while (Yaw < -180.f) Yaw += 360.f;

			// Live convar: *(client.dll + dwSensitivity) -> sensitivity_obj, then +0x58 is the float.
			DWORD64 SensitivityObj = 0;
			if (!ProcessMgr.ReadMemory<DWORD64>(gGame.GetSensitivityAddress(), SensitivityObj) || SensitivityObj == 0)
				return;
			float Sensitivity = 0.f;
			if (!ProcessMgr.ReadMemory<float>(SensitivityObj + Offset::Sensitivity_Value, Sensitivity))
				return;

			// FOV adjust accounts for zoom (scopes etc.); fall back to 1.0 if the read fails.
			float FovAdjust = 1.f;
			ProcessMgr.ReadMemory<float>(Local.Pawn.Address + Offset::Pawn.m_flFOVSensitivityAdjust, FovAdjust);

			constexpr float kYawScale = 0.022f; // engine m_yaw / m_pitch
			float DegPerPixel = Sensitivity * kYawScale * FovAdjust;
			if (DegPerPixel <= 0.f)
				return;

			// Re-use the existing Smooth slider (0..0.9): apply (1 - Smooth) of the delta
			// per frame, identical in spirit to the WriteMemory path.
			float DeltaYaw = Yaw * (1.f - Smooth);
			float DeltaPitch = Pitch * (1.f - Smooth);

			float MoveX = -DeltaYaw / DegPerPixel;
			float MoveY = DeltaPitch / DegPerPixel;

			// Accumulate sub-pixel error across frames so slow aims don't get rounded to zero.
			static float ErrorX = 0.f;
			static float ErrorY = 0.f;
			ErrorX += MoveX;
			ErrorY += MoveY;

			int dx = static_cast<int>(ErrorX);
			int dy = static_cast<int>(ErrorY);
			ErrorX -= static_cast<float>(dx);
			ErrorY -= static_cast<float>(dy);

			if (dx != 0 || dy != 0)
				mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
			return;
		}

		Yaw = Yaw * (1 - Smooth) + Local.Pawn.ViewAngle.y;
		Pitch = Pitch * (1 - Smooth) + Local.Pawn.ViewAngle.x;

		gGame.SetViewAngle(Yaw, Pitch);
	}
}
