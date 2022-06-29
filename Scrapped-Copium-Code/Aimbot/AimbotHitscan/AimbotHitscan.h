#pragma once

#include "../AimbotGlobal/AimbotGlobal.h"

class CAimbotHitscan
{
	int GetHitbox(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon);
	ESortMethod GetSortMethod();
	bool GetTargets(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon);
	bool ScanHitboxes(C_TFPlayer* pLocal, Target_t& target);
	bool ScanHead(C_TFPlayer* pLocal, Target_t& Target);
	bool ScanBuildings(C_TFPlayer* pLocal, Target_t& target);
	bool VerifyTarget(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, Target_t& target);
	bool GetTarget(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, Target_t& outTarget);
	void Aim(CUserCmd* cmd, QAngle& vAngle);
	bool ShouldFire(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& target);

	int PriorityHitbox = 1;

public:
	void Run(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, CUserCmd* pCmd);
};

namespace F { inline CAimbotHitscan Misc; }
