#include "AimbotGlobal.h"
#include "../../Vars.h"


bool CAimbotGlobal::IsKeyDown()
{
	while (GetAsyncKeyState(VK_SHIFT))
		return true;

	return false;
}

void CAimbotGlobal::SortTargets(const ESortMethod& Method)
{
	//sort by preferance
	std::sort(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t& a, const Target_t& b) -> bool {
		switch (Method)
		{
		case ESortMethod::FOV: return (a.m_flFOVTo < b.m_flFOVTo);
		case ESortMethod::DISTANCE: return (a.m_flDistTo < b.m_flDistTo);
		default: return false;
		}
		});

	// sort by priority
	std::sort(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t& a, const Target_t& b) -> bool {
		return (a.n_priority.Mode > b.n_priority.Mode);
		});
}

const Target_t& CAimbotGlobal::GetBestTarget(const ESortMethod& Method)
{
	return *std::min_element(m_vecTargets.begin(), m_vecTargets.end(), [&](const Target_t& a, const Target_t& b) -> bool {
		if (a.n_priority.Mode < b.n_priority.Mode) { return false; }

		switch (Method)
		{
		case ESortMethod::FOV: return (a.m_flFOVTo < b.m_flFOVTo);
		case ESortMethod::DISTANCE: return (a.m_flDistTo < b.m_flDistTo);
		default: return false;
		}
		});
}

bool CAimbotGlobal::ShouldIgnore(C_BaseEntity* pTarget, bool hasMedigun)
{
	player_info_t pInfo{};
	if (!pTarget) { return true; }
	if (!I::EngineClient->GetPlayerInfo(pTarget->entindex(), &pInfo)) { return true; }
	//if (Vars::Aimbot::IgnoreInvunirible && !pTarget->IsVunerable()) { return true; }
	//if (Vars::Aimbot::IgnoreCloaked && pTarget->IsCloaked()) { return true; }
	// ded ringer
	// taunting

	return false;
}

