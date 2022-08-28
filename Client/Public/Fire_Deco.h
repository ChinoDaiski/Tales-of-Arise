#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CFire_Deco final : public CGameObject
{
public:
	typedef struct tagFireDecoDesc
	{
		_float fGlowStrength;
		_float fOriginalStrength;
		_float3 vScale;
		_float3 vLook;
		_float3 vPosition;
		_uint iShaderPass;
		_double iFrameSpeed;
		_tchar* tagTextureCom;
	}FIREDECODESC;
private:
	CFire_Deco(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFire_Deco(const CFire_Deco& rhs);
	virtual ~CFire_Deco() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT SetUp_Components(_tchar* tagTextureCom);
	HRESULT SetUp_ConstantTable();
private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
private:
	_uint m_iMaxFrame = 0;
	_double m_Frame = 0.0;
	_double m_FrameSpeed = 0.0;
private:
	_uint m_iShaderPass = 0;
private:
	_float m_fOriginalStrength = 1.f;
	_float m_fGlowStrength = 1.f;
public:
	static CFire_Deco* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END