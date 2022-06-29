#include "AimbotHitscan.h"
#include "../../Vars.h"

bool IsHitboxValid(int nHitbox, int index)
{
	switch (nHitbox)
	{
	case HITBOX_HEAD: return (index & (1 << 0));
	case HITBOX_PELVIS: return (index & (1 << 1));
	case HITBOX_CHEST: return (index & (1 << 2));
	case HITBOX_BODY:
	case HITBOX_UPPER_CHEST:
	case HITBOX_NECK:
	case HITBOX_LEFT_CALF:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_LEFT_FOOT:
	case HITBOX_RIGHT_THIGH:
	case HITBOX_RIGHT_HAND: return (index & (1 << 3));
	case HITBOX_LOWER_NECK:
	case HITBOX_RIGHT_FOOT: return (index & (1 << 4));
	}
	return false;
};

int CAimbotHitscan::GetHitbox(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon)
{
	switch (Vars::Aimbot::Global::AimHitbox)
	{
		case 0: { return HITBOX_HEAD; }
		case 1: { return HITBOX_BODY; }
		case 2:
		{
		const int nClassNum = pLocal->GetTeamNumber();

		if (nClassNum == TF_CLASS_SNIPER)
		{
			if (pWeapon->m_iItemDefinitionIndex() != Sniper_m_TheSydneySleeper)
			{
				return (pLocal->InCond(TF_COND_ZOOMED) ? HITBOX_HEAD : HITBOX_BODY);
			}

			return HITBOX_BODY;
		}
		if (nClassNum == TF_CLASS_SPY)
		{
			const bool bIsAmbassador = (pWeapon->m_iItemDefinitionIndex() == Spy_m_TheAmbassador || 
				pWeapon->m_iItemDefinitionIndex() == Spy_m_FestiveAmbassador);
			
			return (bIsAmbassador ? HITBOX_HEAD : HITBOX_BODY);
		}
		return HITBOX_BODY;
		
		}
	}

	return HITBOX_HEAD;
}

ESortMethod CAimbotHitscan::GetSortMethod()
{
	switch (Vars::Aimbot::Hitscan::SortMethod)
	{
	case 0: return ESortMethod::FOV;
	case 1: return ESortMethod::DISTANCE;
	default: return ESortMethod::UNKNOWN;
	}
}

bool CAimbotHitscan::GetTargets(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon)
{
	static bool szIgnoreTeam = false;

	const ESortMethod sortMethod = GetSortMethod();

	if (sortMethod == ESortMethod::FOV)
	{
		// do something with menu honestly idk why this is even here
	}

	F::AimbotGlobal.m_vecTargets.clear();

	const Vector vLocalPos = pLocal->Weapon_ShootPosition();
	QAngle vLocalAngles;
	I::EngineClient->GetViewAngles(vLocalAngles);

	//Players
	if (Vars::Aimbot::Global::AimPlayers)
	{
		int n;
		int Team = pLocal->GetTeamNumber();
		int nHitbox = GetHitbox(pLocal, pWeapon);
		// eventually do something with bIsMedigun and figure out how to use TF_WEAPON_MEDIGUN

		for (n = 1; n < g_Globals.m_nMaxClients; n++)
		{
			if (n == g_Globals.m_nLocalIndex)
				continue;

			const auto &pTarget = I::ClientEntityList->GetClientEntity(n);

			if (!pTarget || pTarget->IsDormant())
				continue;

			C_TFPlayer* pPlayer = pTarget->As<C_TFPlayer*>();

			if (!pPlayer || pPlayer->deadflag() || pPlayer->InLocalTeam())
				continue;

			
			if (Vars::Aimbot::Global::IgnoreInvunirible && pPlayer->IsInvulnerable())
				continue;

			if (Vars::Aimbot::Global::IgnoreCloaked && pPlayer->IsCloaked())
				continue;

			if (Vars::Aimbot::Global::IgnoreTaunting && pPlayer->IsTaunting())
				continue;

			PriorityHitbox = nHitbox;

			Vector vPos = Vector();
			pPlayer->GetHitboxPosition(nHitbox, vPos);
			Vector vAngleTo = CalcAngle(vLocalPos, vPos);
			const float flFOVTo = sortMethod == ESortMethod::FOV ? CalcFov(vLocalAngles, vAngleTo) : 0.0f;
			const float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;
			C_TFWeaponBase* pWeapon = pLocal->GetActiveTFWeapon();

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Hitscan::AimFOV.m_Var)
			{
				continue;
			}

			F::AimbotGlobal.m_vecTargets.push_back({ pPlayer, ETargetType::PLAYER, vPos, vAngleTo, flFOVTo, flDistTo, nHitbox});

		}

	}

	if (Vars::Aimbot::Global::AimSentry.m_Var)
	{
		int n;

		for (n = 1; n < g_Globals.m_nMaxClients; n++)
		{
			const auto& pEntity = UTIL_EntityByIndex(n);

			C_ObjectSentrygun* pSentry = pEntity->As<C_ObjectSentrygun*>();

			if (!pSentry->IsAlive())
				continue;

			if (pSentry->InLocalTeam())
				continue;

			Vector vPos = pSentry->WorldSpaceCenter();
			Vector vAngleTo = CalcAngle(vLocalPos, vPos);
			float flFOVTo = sortMethod == ESortMethod::FOV ? CalcFov(vLocalAngles, vAngleTo) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Hitscan::AimFOV.m_Var)
				continue;

			float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			F::AimbotGlobal.m_vecTargets.push_back({ pSentry, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	if (Vars::Aimbot::Global::AimDispencer.m_Var)
	{
		int n;

		for (n = 1; n < g_Globals.m_nMaxClients; n++)
		{
			const auto& pEntity = UTIL_EntityByIndex(n);

			C_ObjectDispenser* pDisp = pEntity->As<C_ObjectDispenser*>();

			if (!pDisp->IsAlive())
				continue;

			if (pDisp->InLocalTeam())
				continue;

			Vector vPos = pDisp->WorldSpaceCenter();
			Vector vAngleTo = CalcAngle(vLocalPos, vPos);
			float flFOVTo = sortMethod == ESortMethod::FOV ? CalcFov(vLocalAngles, vAngleTo) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Hitscan::AimFOV.m_Var)
				continue;

			float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			F::AimbotGlobal.m_vecTargets.push_back({ pDisp, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	if (Vars::Aimbot::Global::AimTele.m_Var)
	{
		int n;

		for (n = 1; n < g_Globals.m_nMaxClients; n++)
		{
			const auto& pEntity = UTIL_EntityByIndex(n);

			C_ObjectTeleporter* pTele = pEntity->As<C_ObjectTeleporter*>();

			if (!pTele->IsAlive())
				continue;

			if (pTele->InLocalTeam())
				continue;

			Vector vPos = pTele->WorldSpaceCenter();
			Vector vAngleTo = CalcAngle(vLocalPos, vPos);
			float flFOVTo = sortMethod == ESortMethod::FOV ? CalcFov(vLocalAngles, vAngleTo) : 0.0f;

			if (sortMethod == ESortMethod::FOV && flFOVTo > Vars::Aimbot::Hitscan::AimFOV.m_Var)
				continue;

			float flDistTo = sortMethod == ESortMethod::DISTANCE ? vLocalPos.DistTo(vPos) : 0.0f;

			F::AimbotGlobal.m_vecTargets.push_back({ pTele, ETargetType::BUILDING, vPos, vAngleTo, flFOVTo, flDistTo });
		}
	}

	return !F::AimbotGlobal.m_vecTargets.empty();
}

bool CAimbotHitscan::ScanHitboxes(C_TFPlayer* pLocal, Target_t& Target)
{
	if (Target.m_TargetType == ETargetType::PLAYER)
	{
		if (!Vars::Aimbot::Hitscan::ScanHitboxes.m_Var)
			return false;
	}

	else if (Target.m_TargetType == ETargetType::BUILDING)
	{
		if (!Vars::Aimbot::Hitscan::ScanBuildings.m_Var)
			return false;
	}

	Vector vLocalPos = pLocal->Weapon_ShootPosition();

	for (int nHitbox = Target.m_TargetType == ETargetType::PLAYER ? 1 : 0; nHitbox < Target.m_pEntity->As<C_TFPlayer*>()->m_nHitboxSet(); nHitbox++)
	{
		if (Target.m_TargetType == ETargetType::PLAYER && nHitbox == HITBOX_PELVIS)
			continue;

		Vector vHitbox = Vector();
		Target.m_pEntity->As<C_TFPlayer*>()->GetHitboxPosition(nHitbox, vHitbox);

		if (G::Util.VisPos(pLocal, Target.m_pEntity->As<C_BaseEntity*>(), vLocalPos, vHitbox))
		{
			Target.m_vPos = vHitbox;
			Target.m_vAngleTo = CalcAngle(vLocalPos, vHitbox);
			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::ScanHead(C_TFPlayer* pLocal, Target_t& Target)
{
	if (!Vars::Aimbot::Hitscan::ScanHead.m_Var)
		return false;

	const model_t *pModel = Target.m_pEntity->GetModel();
	if (!pModel)
		return false;

	studiohdr_t* pHDR = reinterpret_cast<studiohdr_t*>(I::ModelInfoClient->GetStudiomodel(pModel));
	if (!pHDR)
		return false;

	matrix3x4_t BoneMatrix[128];
	if (!Target.m_pEntity->SetupBones(BoneMatrix, 128, 0x100, I::GlobalVars->curtime))
		return false;

	mstudiohitboxset_t* pSet = pHDR->pHitboxSet(Target.m_pEntity->As<C_TFPlayer*>()->m_nHitboxSet());
	if (!pSet)
		return false;

	mstudiobbox_t* pBox = pSet->pHitbox(HITBOX_HEAD);
	if (!pBox)
		return false;

	Vector vLocalPos = pLocal->Weapon_ShootPosition();
	Vector vMins = pBox->bbmin;
	Vector vMaxs = pBox->bbmax;

	const float fScale = 0.8f;
	const std::vector<Vector> vecPoints = {
		Vector(((vMins.x + vMaxs.x) * 0.5f), (vMins.y * fScale), ((vMins.z + vMaxs.z) * 0.5f)),
		Vector((vMins.x * fScale), ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vector((vMaxs.x * fScale), ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f))
	};

	for (const auto& Point : vecPoints)
	{
		Vector vTransformed = {};
		VectorTransform(Point, BoneMatrix[pBox->bone], vTransformed);

		if (G::Util.VisPosHitboxId(pLocal, Target.m_pEntity->As<C_BaseEntity*>(), vLocalPos, vTransformed, HITBOX_HEAD)) {
			Target.m_vPos = vTransformed;
			Target.m_vAngleTo = CalcAngle(vLocalPos, vTransformed);
			Target.m_bHasMultiPointed = true;
			return true;
		}
	}
	return false;
}

bool CAimbotHitscan::ScanBuildings(C_TFPlayer* pLocal, Target_t& Target)
{
	if (!Vars::Aimbot::Hitscan::ScanBuildings.m_Var)
		return false;

	Vector vLocalPos = pLocal->Weapon_ShootPosition();

	Vector vMins = Target.m_pEntity->As<C_ObjectSentrygun*>()->m_vecBuildMins();
	Vector vMaxs = Target.m_pEntity->As<C_ObjectSentrygun*>()->m_vecBuildMaxs();

	const std::vector<Vector> vecPoints = {
		Vector(vMins.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vector(vMaxs.x * 0.9f, ((vMins.y + vMaxs.y) * 0.5f), ((vMins.z + vMaxs.z) * 0.5f)),
		Vector(((vMins.x + vMaxs.x) * 0.5f), vMins.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
		Vector(((vMins.x + vMaxs.x) * 0.5f), vMaxs.y * 0.9f, ((vMins.z + vMaxs.z) * 0.5f)),
		Vector(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMins.z * 0.9f),
		Vector(((vMins.x + vMaxs.x) * 0.5f), ((vMins.y + vMaxs.y) * 0.5f), vMaxs.z * 0.9f)
	};

	//const matrix3x4_t& Transform = Target.m_pEntity->As<C_ObjectSentrygun*>();

	for (const auto& Point : vecPoints)
	{
		Vector vTransformed = {};
		//VectorTransform(Point, Transform, vTransformed);

		if (G::Util.VisPos(pLocal, Target.m_pEntity->As<C_ObjectSentrygun*>(), vLocalPos, vTransformed)) {
			Target.m_vPos = vTransformed;
			Target.m_vAngleTo = CalcAngle(vLocalPos, vTransformed);
			return true;
		}
	}

	return false;
}

bool CAimbotHitscan::VerifyTarget(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, Target_t& Target)
{
	switch (Target.m_TargetType)
	{
	case ETargetType::PLAYER:
	{
		if (Target.m_nAimedHitbox == HITBOX_HEAD)
		{
			int nHit = -1;

			if (!G::Util.VisPosHitboxIdOut(pLocal, Target.m_pEntity->As<C_BaseEntity*>(), pLocal->Weapon_ShootPosition(), Target.m_vPos, nHit))
				return false;

			if (nHit != HITBOX_HEAD && !ScanHead(pLocal, Target))
				return false;
		}

		else if (Target.m_nAimedHitbox == HITBOX_PELVIS)
		{
			if (G::Util.VisPos(pLocal, Target.m_pEntity->As<C_BaseEntity*>(), pLocal->Weapon_ShootPosition(), Target.m_vPos) && !ScanHitboxes(pLocal, Target))
				return false;
		}

		break;
	}

	case ETargetType::BUILDING:
	{
		//if (!G::Util.VisPos(pLocal, Target.m_pEntity->As<C_BaseEntity*>(), pLocal->Weapon_ShootPosition(), Target.m_vPos))
		//{
			//Sentryguns have hitboxes, it's better to use ScanHitboxes for them
		//	if (Target.m_pEntity->As<C_ObjectSentrygun*>()->GetClientClass() == ETFClassID::CObjectSentrygun ? !ScanHitboxes(pLocal, Target) : !ScanBuildings(pLocal, Target))
		//		return false;
		//}

		break;
	}

	default:
	{
		if (!G::Util.VisPos(pLocal, Target.m_pEntity->As<C_BaseEntity*>(), pLocal->Weapon_ShootPosition(), Target.m_vPos))
			return false;

		break;
	}
	}

	return true;
}

bool CAimbotHitscan::GetTarget(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, Target_t& Out)
{
	if (!GetTargets(pLocal, pWeapon))
		return false;

	F::AimbotGlobal.SortTargets(GetSortMethod());

	for (auto& Target : F::AimbotGlobal.m_vecTargets)
	{
		if (!VerifyTarget(pLocal, pWeapon, Target))
			continue;

		Out = Target;
		return true;
	}

	return false;
}

void CAimbotHitscan::Aim(CUserCmd* pCmd, QAngle& vAngle)
{
	QAngle ang;
	I::EngineClient->GetViewAngles(ang);

	QAngle ang2;
	I::ClientPrediction->GetLocalViewAngles(ang2);

	vAngle -= ang2;
	ClampAngles(vAngle);

	int nAimMethod = Vars::Aimbot::Global::AimMethod.m_Var;

	switch (nAimMethod)
	{
	case 0: //Plain
	{
		pCmd->viewangles = vAngle;
		I::EngineClient->SetViewAngles(pCmd->viewangles);
		break;
	}

	case 1: //Smooth
	{
		//Calculate delta of current viewangles and wanted angles
		QAngle vecDelta = vAngle - ang;

		//Clamp, keep the angle in possible bounds
		ClampAngles(vecDelta);

		//Basic smooth by dividing the delta by wanted smooth amount
		pCmd->viewangles += vecDelta / Vars::Aimbot::Hitscan::SmoothingAmount.m_Var;

		//Set the viewangles from engine
		I::EngineClient->SetViewAngles(pCmd->viewangles);
		break;
	}

	case 2: //Silent
	{
		G::Util.FixMovement(vAngle, pCmd);
		pCmd->viewangles = vAngle;
		break;
	}

	default: break;
	}
}

bool CAimbotHitscan::ShouldFire(C_TFPlayer* pLocal, C_BaseCombatWeapon* pWeapon, CUserCmd* pCmd, const Target_t& Target)
{
	if (!Vars::Aimbot::Global::AutoShoot.m_Var)
		return false;

	switch (pWeapon->m_iItemDefinitionIndex())
	{
	case Sniper_m_TheMachina:
	case Sniper_m_ShootingStar:
	{
		if (!pLocal->IsZoomed())
			return false;

		break;
	}
	default: break;
	}

	switch (pLocal->m_iClass())
	{
	case TF_CLASS_SNIPER:
	{
		bool bIsScoped = pLocal->IsZoomed();

		if (Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var)
		{
			if (pWeapon->m_iItemDefinitionIndex() != Sniper_m_TheClassic
				&& pWeapon->m_iItemDefinitionIndex() != Sniper_m_TheSydneySleeper)
				return false;
		}

		if (Vars::Aimbot::Hitscan::WaitForCharge.m_Var && bIsScoped)
		{
			int nHealth = Target.m_pEntity->As<C_BaseEntity*>()->GetHealth();
			bool bIsCritBoosted = pLocal->IsCritBoosted();

			if (Target.m_nAimedHitbox == HITBOX_HEAD && pWeapon->m_iItemDefinitionIndex() != Sniper_m_TheSydneySleeper)
			{
				if (nHealth > 150)
				{
					float flDamage = RemapValClamped(pWeapon->GetAttackDamageScale(), 0.0f, 150.0f, 0.0f, 450.0f);
					int nDamage = static_cast<int>(flDamage);

					if (nDamage < nHealth && nDamage != 450)
						return false;
				}

				else
				{
					if (!bIsCritBoosted /* && can weapon headshot */ )
						return false;
				}
			}

			else
			{
				if (nHealth > (bIsCritBoosted ? 150 : 50))
				{
					float flMult = Target.m_pEntity->As<C_TFPlayer*>() ? 1.36f : 1.0f;

					if (bIsCritBoosted)
						flMult = 3.0f;

					float flMax = 150.0f * flMult;
					int nDamage = static_cast<int>(pWeapon->GetAttackDamageScale() * flMult);

					if (nDamage < Target.m_pEntity->As<C_BaseEntity*>()->GetHealth() && nDamage != static_cast<int>(flMax))
						return false;
				}
			}
		}

		break;
	}

	case TF_CLASS_SPY:
	{
		if (Vars::Aimbot::Hitscan::WaitForHeadshot.m_Var  /* && !g_GlobalInfo.m_bWeaponCanHeadShot */ )
		{
			if (pWeapon->m_iItemDefinitionIndex() == Spy_m_TheAmbassador || pWeapon->m_iItemDefinitionIndex() == Spy_m_FestiveAmbassador)
				return false;
		}

		break;
	}

	default: break;
	}

	int nAimMethod = (Vars::Aimbot::Hitscan::SpectatedSmooth.m_Var  /* && g_GlobalInfo.m_bLocalSpectated*/) ? 1 : Vars::Aimbot::Global::AimMethod.m_Var;

	if (nAimMethod == 1)
	{
		Vector vForward = { };
		AngleVectors(pCmd->viewangles, &vForward);
		Vector vTraceStart = pLocal->Weapon_ShootPosition();
		Vector vTraceEnd = (vTraceStart + (vForward * 8192.0f));

		trace_t trace;
		CTraceFilterSimpleList filter = NULL;
		filter.AddEntityToIgnore(pLocal);

		UTIL_TraceLine(vTraceStart, vTraceEnd, (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);

		if (trace.m_pEnt != Target.m_pEntity)
			return false;

		if (Target.m_nAimedHitbox == HITBOX_HEAD)
		{
			if (trace.hitbox != HITBOX_HEAD)
				return false;

			if (!Target.m_bHasMultiPointed)
			{
				Vector vMins = {}, vMaxs = {}, vCenter = {};
				matrix3x4_t Matrix = {};

				if (!Target.m_pEntity->As<C_BaseEntity*>()->GetHitboxMinsAndMaxsAndMatrix(HITBOX_HEAD, vMins, vMaxs, Matrix, &vCenter))
					return false;

				vMins *= 0.5f;
				vMaxs *= 0.5f;

				Ray_t ray;

				if (!RayToOBB(vTraceStart, vForward, vCenter, vMins, vMaxs, Matrix))
					return false;
			}
		}
	}

	return true;
}