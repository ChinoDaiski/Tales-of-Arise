#pragma once
#include "Client_Defines.h"
#include "Enemy.h"



BEGIN(Client)

class CFire_Avatar final : public CEnemy
{
	enum EFR_ANIM_STATE
	{
		ARISE_B, ARISE_F, ATTACK_APPERANCE_SUB, ATTACK_BEAM, CLEAVE_LEFT, CLEAVE_RIGHT, CHRUSH_BOTH_ARMS,
		CRUSH_CENTER, CRUSH_LEFT, CRUSH_PILLAR, CRUSH_RIGHT, FIRE_BALL, FIRE_BALL_PILLAR_END, FIRE_BALL_PILLAR_LOOP, FIRE_BALL_PILLAR_START,
		ATTACK_ROCK_THROW, ATTACK_STRIKE_DOWN_END, ATTACK_STRIKE_DOWN_LOOP, DEAD, DOWN_B, DOWN_F,
		MAGIC_EMIT, MAGIC_END, MAGIC_LOOP, MAGIC_START, MOVE_IDLE
	};

private:
	explicit CFire_Avatar(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CFire_Avatar(const CFire_Avatar& rhs);
	virtual ~CFire_Avatar() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

private:
	HRESULT Ready_Effect(_tchar* tagLayer);

private:
	void Apperance();		// 땅 밑에서 등장모션
	void Fire_Ball(_double TimeDelta);		// 두손모아 운석 쏘는 모션
	void SweepGround(_double TimeDelta);	// 손바닥으로 수직으로 세워서 땅 쓸어버리는 모션
	void RockFalling(_double TimeDelta);	// 돌 집어들어 공중에서 부셔서 공격하는 모션
	void Punch_Ground(_double TimeDelta);	// 땅 내리치는 모션
	void TwoHandPunch_Ground(_double TimeDelta);	// 땅 내리치는 모션
	//void Laser_Beam(_double TimeDelta);		// 입에서 레이저
	void NoMoreLife(_double TimeDelta);
private:
	CModel*				m_pModelCom = nullptr;
	CModel*				m_pModelSKL = nullptr;

public:
	static CFire_Avatar* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	_bool Get_CrashRock() { return m_bCrashRock; }

private:
	class CCamera_Default* m_pCamera = nullptr;
	_vector m_vStorePos = { 0.f, 0.f, 0.f, 1.f };
	_uint numAnim = 0;
	_uint m_iMotion = -1;
	_double m_TimeDelta = 0.0;
	_bool m_bTime = false;
	_bool m_bStart = false;
	_bool m_bOnce = false;
	_bool m_bGo = false;
	_bool m_bAppearanceEnd = false;
	_bool m_bCreateRock = false;
	_bool m_bCrashRock = false;
	_bool m_bCreateSmallRock = false;
	_bool m_bNoHP = false;
	_uint m_balsephHP = 0;

public:
	void Set_Rim(_float3 vPos) {
		m_vRimPosition.x = vPos.x;
		m_vRimPosition.y = vPos.y;
		m_vRimPosition.z = vPos.z;
	}
	_float4 Get_Rim() {
		return m_vRimPosition;
	}

	void Set_RimWidth(_float fWidth) {
		m_fRimWidth = fWidth;
	}
	_float Get_RimWidth() {
		return m_fRimWidth;
	}

private:
	_float4 m_vRimPosition = _float4(1000.f, 500.f, 100.f, 1.f);
	_float m_fRimWidth = 0.7f;
};

END