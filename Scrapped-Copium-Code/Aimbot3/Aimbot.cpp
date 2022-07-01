#define NOMINMAX 
#include "aimbot.h"
#include "../Bruh/Extern.h"
#include <math.h>
#include "../Vars.h"

	C_BaseEntity* get_closest_to_crosshair(C_TFPlayer* local_player, const Vector view_angles, int settings_fov) {
		C_BaseEntity* best_entity = nullptr;
		float max_delta = FLT_MAX;

		for (int i = 1; i < (I::EngineClient->GetMaxClients() + 1); i++) {
			auto entity = I::ClientEntityList->GetClientEntity(i);

			if (!entity ||  entity->IsDormant()) {
				continue;
			}

			ClientClass* pCC = entity->GetClientClass();

			if (!pCC)
				continue;

			switch (pCC->GetTFClientClass())
			{
				case ETFClientClass::CTFPlayer:
				{
					C_TFPlayer* pPlayer = entity->As<C_TFPlayer*>();

					if (pPlayer->deadflag() ||  pPlayer->InLocalTeam())
						break;

					if (!pPlayer->IsVisible(local_player))
						break;

					float delta = CalcAngle(local_player->EyePosition(), pPlayer->EyePosition()/*, view_angles*/).Length();
					if (delta < max_delta && delta < settings_fov) {
						max_delta = delta;
						best_entity = pPlayer;
					}
				}
			}
		}
		return best_entity;
	}

	C_BaseEntity* get_closest_distance(C_TFPlayer* local_player) {
		return nullptr;
	}

	int get_closest_hibox(C_TFPlayer* local_player, C_BaseEntity* target, const Vector view_angles) {
		float max_delta = FLT_MAX;
		int best_hitbox = NULL;

		for (int i = HITBOX_HEAD; i != HITBOX_RIGHT_FOREARM; i++) {
			float delta = CalcAngle(local_player->Weapon_ShootPosition(), target->GetHitboxPosition(i)/*, view_angles*/).Length();
			if (delta < max_delta) {
				max_delta = delta;
				best_hitbox = i;
			}
		}
		return best_hitbox;
	}

	bool is_position_visible(C_BaseEntity* local_player, const Vector& source) {
		trace_t trace;
		Ray_t ray;			 // the future of variable naming
		CTraceFilterSimpleList filter = NULL;

		filter.AddEntityToIgnore(local_player);

		auto local_eye = local_player->EyePosition();
		ray.Init(local_eye, source);

		I::EngineTrace->TraceRay(ray, MASK_SOLID, &filter, &trace);

		return !trace.DidHit();
	}

	Vector predict_position(const Vector& source, const Vector& destination, const Vector& target_velocity, const float projectile_speed, const float projectile_gravity) {
		const auto distance = source.DistTo(destination);
		const auto travel_time = distance / projectile_speed;
		auto pred_destination = destination + (target_velocity)*travel_time;

		pred_destination.y += 0.5f * fabsf(projectile_gravity) * travel_time * travel_time;

		return pred_destination;
	}

	void CAimbot::Run(C_TFPlayer* local_player, CUserCmd* cmd) 
	{
		if (!GetAsyncKeyState(VK_SHIFT))
			return;

		if (local_player->IsInvulnerable() || local_player->IsTaunting()) {
			return;
		}

		float projectile_speed = 0.0f, projectile_gravity = 0.0f;
		auto is_projectile_weapon = G::Util.GetProjectileInfo(local_player, projectile_speed, projectile_gravity);

		auto target = get_closest_to_crosshair(local_player, G::Util.VecViewAngles(cmd), is_projectile_weapon ? Vars::Aimbot::Hitscan::AimFOV.m_Var : Vars::Aimbot::Hitscan::AimFOV.m_Var);
		if (!target) {
			return;
		}

		const auto hitbox_id = Vars::Aimbot::Global::AimHitbox == 0 ? get_closest_hibox(local_player, target, G::Util.VecViewAngles(cmd)) : HITBOX_HEAD;

		if (is_projectile_weapon) {
			 //projectile
		
			auto target_position = target->GetHitboxPosition(hitbox_id);
			if (local_player->m_iClass() == TF_CLASS_SOLDIER) {
				target_position = /*(target->GetAbsOrigin() + Vector(0, 0, 15.0f)*/ target->GetAbsOrigin();
			}
		
			auto predicited_position = predict_position(local_player->EyePosition(), target_position, target->GetVelocity(), projectile_speed, projectile_gravity);
			if (!is_position_visible(local_player, predicited_position)) {
				return;
			}

			auto angle = G::Util.CalcAngleProjectile(local_player->EyePosition(), predicited_position);
			QAngle myAngle = QAngle(angle.x, angle.y, angle.z);
			ClampAngles(myAngle);
		
			if (Vars::Aimbot::Global::SmoothDuration > 0) {
				myAngle /= (Vars::Aimbot::Global::SmoothDuration * 5);
			}
		
			I::EngineClient->SetViewAngles( myAngle );
			//cmd->viewangles = myAngle;
		}
		else {
		// normal

			auto angle = CalcAngle(local_player->EyePosition(), target->GetHitboxPosition(hitbox_id)/*, cmd->viewangles*/);
			QAngle myAngle = QAngle(angle.x, angle.y, angle.z);
			ClampAngles(myAngle);

			if (Vars::Aimbot::Global::SmoothDuration > 0) {
				myAngle /= (Vars::Aimbot::Global::SmoothDuration * 5);
			}

			I::EngineClient->SetViewAngles(myAngle);
			//cmd->viewangles += myAngle;
		 }
	}