#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CDissolve final : public CGameObject
{
private:
	CDissolve(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDissolve(const CDissolve& rhs);
	virtual ~CDissolve() = default;
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
	CTexture* m_pSourDissolveTextureCom = nullptr;
	CTexture* m_pDestDissolveTextureCom = nullptr;
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
private:
	_float m_fWinCX = 0.f, m_fWinCY = 0.f;
	_float4x4 m_ProjMatrix;
private:
	_float4 m_vClearColor;
private:
	_float4 m_vDissolveColor = _float4(242 / 255.f, 134 / 255.f, 2 / 255.f, 0.f);
private:
	_bool m_bStart = false;
	_double m_DissolveTime = 2.0, m_DissolveTimeAcc = 0.0;
public:
	static CDissolve* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END