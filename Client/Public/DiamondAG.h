#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END
BEGIN(Client)
class DiamondAG  final : public CGameObject
{
public:
	explicit DiamondAG(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit DiamondAG(const DiamondAG& rhs);
	virtual ~DiamondAG() =default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	void SetRender(bool what) { m_isRender = what; }
	typedef struct MOVINGHPDESC {
		///CTransform
		CTransform* targetTransform;
		float pivotx, pivoty;
		int kind;
	}MHDESC;
private:
	int m_kind;
	bool m_firstMade = false;
	_float percent = 0;
	_float hp = 0;
	_float m_fMaxAG = 0;
	_float m_fCurAG = 0;
	_float m_isRender = true;
	_float m_fAgPercnet = 0;
	_float maxhp;
	bool m_isBlue = false;
	float m_pivotx = 0, m_pivoty = 0;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	_float4x4			m_ProjMatrix;
	_float m_fWinCX = 0.f, m_fWinCY = 0.f;

	_float				m_Redbar = 1.f;
	float time;
	bool Timegone = false;
private:
	_double				m_Frame = 0.0;
	_float m_fX, m_fY;
	bool m_bRender = false;

public:
	HRESULT SetUp_Components();
	CTransform* TargetTransform = nullptr;
	void SetTarget(CTransform* targetTransform) { TargetTransform = targetTransform; }

public:
	static DiamondAG* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};
END

