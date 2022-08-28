#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)

class CShader;
class CCollider;
class CRenderer;
class CModel;

END

BEGIN(Client)

class CPlayer;
class CLord_Balseph;

class CBalseph_Stair final : public CGameObject {
public:
	typedef struct _tagBalseph_StairDesc {
		_float4x4	TransformMatrix;	// 이동 행렬
		const TCHAR* pModelTag = nullptr;		// 모델 이름
		CLord_Balseph* pTargetEnemy = nullptr;	// 타겟이 되는 적 오브젝트 정보(해당 적의 체력이 최대 체력의 반이 됬을때, 계단이 내려온다.)
		CPlayer* pTargetPlayer = nullptr;		// 타겟이 되는 플레이어 오브젝트 정보(계단이 내려왔을 때, WayPoint를 해당 플레이어의 인자를 받아 넘긴다.)
	}Balseph_StairDesc;

private:
	CBalseph_Stair(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	CBalseph_Stair(const CBalseph_Stair& rhs);
	virtual ~CBalseph_Stair() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_ConstantTable();

public:
	static CBalseph_Stair* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	void Activate_Stair(void) { m_bActivate = true; }
	void Set_EnemyTarget(CLord_Balseph* pTarget) { m_pTargetEnemy = pTarget; }

private:
	_bool m_bActivate = false;
	CLord_Balseph* m_pTargetEnemy = nullptr;
	CPlayer* m_pTargetPlayer = nullptr;
};

END