#pragma once
#include"Client_Defines.h"
#include"GameObject.h"
#include<random>
BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END
BEGIN(Client)
class NotMovingFont   final : public CGameObject
{
public:
	explicit NotMovingFont(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit NotMovingFont(const NotMovingFont &rhs);
	virtual ~NotMovingFont() = default;

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
		_float scaleX, scaleY;
		int UIKind = 0;
	}MHDESC;
private:
	default_random_engine dre;
	uniform_int_distribution<> uidX;
	random_device rd;
	string m_str{};
	int m_kind;//sprite¹øÈ£
	int m_UIKind = 0;
	_float m_ScaleX = 0.f, m_ScaleY = 0.f;
	_float percent = 0;
	_int hp = 0;
	_float m_fMaxAG = 0;
	_float m_fCurAG = 0;
	_float m_isRender = true;
	_float maxhp;
	float m_pivotx = 0, m_pivoty = 0;
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;
	_float4x4			m_ProjMatrix;
	_float m_fWinCX = 0.f, m_fWinCY = 0.f;

	_float				m_Redbar = 1.f;
	float m_ftime = 0;
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
	static NotMovingFont* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END
