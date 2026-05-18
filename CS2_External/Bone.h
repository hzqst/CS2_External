#pragma once
#include <vector>
#include "Game.h"

enum BONEINDEX : int
{
	ORIGIN = 0,
	PELVIS = 1,
	SPINE_0 = 2,
	SPINE_1 = 3,
	SPINE_2 = 4,
	NECK_0 = 6,
	HEAD = 7,
	CLAVICLE_L = 8,
	ARM_UPPER_L = 9,
	ARM_LOWER_L = 10,
	HAND_L = 11,
	CLAVICLE_R = 12,
	ARM_UPPER_R = 13,
	ARM_LOWER_R = 14,
	HAND_R = 15,
	HIP_L = 17,
	KNEE_L = 18,
	ANKLE_L = 19,
	HIP_R = 20,
	KNEE_R = 21,
	ANKLE_R = 22,
	CHEST = 23,
};

struct BoneJointData
{
	Vec3 Pos;
	char pad[0x14];
};

struct BoneJointPos
{
	Vec3 Pos;
	Vec2 ScreenPos;
	bool IsVisible = false;
};

class CBone
{
private:
	DWORD64 EntityPawnAddress = 0;
public:
	std::vector<BoneJointPos> BonePosList;

	bool UpdateAllBoneData(const DWORD64& EntityPawnAddress);
};

namespace BoneJointList
{
	// trunk: head -> neck -> chest -> mid spine -> lower spine -> pelvis
	inline std::list<int> Trunk = { HEAD, NECK_0, CHEST, SPINE_2, SPINE_1, PELVIS };
	// left arm: chest -> clavicle -> upper -> lower -> hand
	inline std::list<int> LeftArm = { CHEST, CLAVICLE_L, ARM_UPPER_L, ARM_LOWER_L, HAND_L };
	// right arm: chest -> clavicle -> upper -> lower -> hand
	inline std::list<int> RightArm = { CHEST, CLAVICLE_R, ARM_UPPER_R, ARM_LOWER_R, HAND_R };
	// left leg: pelvis -> hip -> knee -> ankle
	inline std::list<int> LeftLeg = { PELVIS, HIP_L, KNEE_L, ANKLE_L };
	// right leg: pelvis -> hip -> knee -> ankle
	inline std::list<int> RightLeg = { PELVIS, HIP_R, KNEE_R, ANKLE_R };
	// all chains
	inline std::vector<std::list<int>> List = { Trunk, LeftArm, RightArm, LeftLeg, RightLeg };
}