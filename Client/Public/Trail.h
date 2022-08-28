#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Trail.h"
#include "GameInstance.h"
#include "Transform.h"

BEGIN(Client)

class CTrail final : public CGameObject
{
private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;
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
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Trail* m_pVIBufferCom = nullptr;
public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END