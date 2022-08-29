#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CDistortion final : public CGameObject
{
public:
	typedef struct tagDistortionDesc
	{
		CGameObject* pTargetObject;
		_float2 vRevicePosition;
		_float3 vScale;
		_uint iTexture;
	}DISTORTIONDESC;
private:
	CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDistortion(const CDistortion& rhs);
	virtual ~CDistortion() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
public:
	_bool Get_Render();
	void Set_Render(_bool bRender);
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();
private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
private:
	DISTORTIONDESC m_tDistortionDesc;
	_bool m_bRender = false;
	_double m_PlayTime = 0.0;
public:
	static CDistortion* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END