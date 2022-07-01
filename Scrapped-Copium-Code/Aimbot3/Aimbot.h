#pragma once
#include "../../SDK/SDK.h"

class CAimbot
{
public:
	void Run(C_TFPlayer* pLocal, CUserCmd* cmd);
};

namespace F { inline CAimbot Aimbot; }

//entity == local_player || !entity->is_alive() ||