#pragma once
#include "../../../SDK/SDK.h"

#undef min
#undef max

	enum struct ETargetType {
		UNKNOWN,
		PLAYER,
		BUILDING,
		STICKY
	};

	enum struct ESortMethod {
		UNKNOWN,
		FOV,
		DISTANCE
	};

	enum Ignored {
		INVUL = 1 << 0,
		CLOAKED = 1 << 1,
		DEADRINGER = 1 << 2,
		FRIENDS = 1 << 3,
		TAUNTING = 1 << 4
	};


	struct Target_t {
		IClientEntity* m_pEntity = nullptr; //Carefull with using IClientEntity, shit could get messy
		ETargetType m_TargetType = ETargetType::UNKNOWN;
		Vector m_vPos = {};
		Vector m_vAngleTo = {};
		float m_flFOVTo = std::numeric_limits<float>::max();
		float m_flDistTo = std::numeric_limits<float>::max();
		int m_nAimedHitbox = -1;
		bool m_bHasMultiPointed = false;
		Priority n_priority = {};
	};

	class CAimbotGlobal {
	public:
		bool IsKeyDown();
		void SortTargets(const ESortMethod& Method);
		const Target_t& GetBestTarget(const ESortMethod& Method);
		bool ShouldIgnore(C_BaseEntity* pTarget, bool hasMedigun = false);

		std::vector<Target_t> m_vecTargets = {};
	};

	namespace F { inline CAimbotGlobal AimbotGlobal; }