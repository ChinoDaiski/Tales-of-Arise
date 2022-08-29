#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "MeshEffect.h"
BEGIN(Engine)
class CShader;
class CCollider;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CMeteo final : public CGameObject
{
private:
	explicit CMeteo(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CMeteo(const CMeteo& rhs);
	virtual ~CMeteo() = default;

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
	CCollider*			m_pSphereCom = nullptr;
	CMeshEffect* EffectMesh = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

public:
	static CMeteo* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	_double m_DeleteTime = 0.0;
	_bool m_bOnlyOnce = false;
	_vector vPos = { 0.f, 0.f, 0.f, 1.f };
};

END