#include "../../SDK/SDK.h"

class CFeatures_Aimbot
{
public:
	void Run(CUserCmd* cmd);

private:
	bool ShouldRun(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon);
};