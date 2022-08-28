#pragma once
#include "Client_Defines.h"
#include "BlendObject.h"

BEGIN(Client)

class CFire_Effect final : public CBlendObject
{
public:
	typedef struct tagFireEffectDesc
	{
		CTransform* pTransformCom;
		_tchar* tagTextureCom;
		CModel* pSocketModel;
		const char* tagBone;
		_bool bChase;
		_bool bRandom;
		_uint iShaderPass;
		_float fOriginalStrength;
		_float fGlowStrength;
		_double FrameSpeed;
		_float3 vScale;
	}FIREEFFECTDESC;
private:
	CFire_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CFire_Effect(const CFire_Effect& rhs);
	virtual ~CFire_Effect() = default;
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
	HRESULT Restart_FireEffect();
private:
	CRenderer* m_pRendererCom = nullptr;
	CShader* m_pShaderCom = nullptr;
	CTexture* m_pTextureCom = nullptr;
	CVIBuffer_Rect* m_pVIBufferCom = nullptr;
private:
	FIREEFFECTDESC m_tFireEffectDesc;
private:
	_uint m_iMaxFrame = 0;
	_double FrameSpeed = 0.0;
	_double m_Frame = 0.0;
private:
	_float m_fReviceX = 0.f;
	_float m_fReviceY = 0.f;
private:
	_uint m_iRandomPositionType = 0;
public:
	static CFire_Effect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END