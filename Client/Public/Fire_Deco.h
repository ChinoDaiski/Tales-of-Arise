#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CFire_Deco final : public CGameObject {

public:
	typedef struct tagFireDecoDesc{
		_float fGlowStrength;
		_float fOriginalStrength;
		_float3 vScale;
		_float3 vLook;
		_float3 vPosition;
		_uint iShaderPass;
		_double iFrameSpeed;
		_tchar* tagTextureCom;
		_float3 fRotation = _float3(0.f, 0.f, 0.f);
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

public:
	_double Get_FrameSpeed(void) { return m_FrameSpeed; }
	void Set_FrameSpeed(_double dFrameSpeed) { m_FrameSpeed = dFrameSpeed; }

private:
	_uint m_iMaxFrame = 0;
	_double m_Frame = 0.0;
	_double m_FrameSpeed = 0.0;

private:
	_uint m_iShaderPass = 0;

public:
	_float Get_OriginalStrength(void) { return m_fOriginalStrength; }
	void Set_OriginalStrength(_float fOriginalStrength) { m_fOriginalStrength = fOriginalStrength; }

	_float Get_GlowStrength(void) { return m_fGlowStrength; }
	void Set_GlowStrength(_float fGlowStrength) { m_fGlowStrength = fGlowStrength; }


private:
	_float m_fOriginalStrength = 1.f;
	_float m_fGlowStrength = 1.f;

public:
	static CFire_Deco* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	_uint Get_FireNumber(void) { return m_iCurFireNumber; }

private:
	_uint m_iCurFireNumber;
	static _uint m_iFireNumber;

public:
	// 회전 정도를 가져온다.
	const _float3 Get_Rotation(void) { return m_fRotation; }
	void Set_Rotation(_float3 fRotation) { m_fRotation = fRotation; }

private:
	// 회전 정보
	_float3 m_fRotation = _float3{ 0.f, 0.f, 0.f };

public:
	void Set_Target(CGameObject* pTargetObject) { m_pTarget = pTargetObject; }

private:
	CGameObject* m_pTarget = nullptr;		// 타겟 오브젝트, 해당 타겟의 x,z방향으로 look을 돌린다.

public:
	void Set_Scale(_float3 vScale) { m_fScale = vScale; }
	_float3 Get_Scale(void) { return m_fScale; }

private:
	_float3 m_fScale;	// 크기 범위
};

END