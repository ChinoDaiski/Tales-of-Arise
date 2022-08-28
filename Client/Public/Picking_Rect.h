#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

BEGIN(Client)

class CPicking_Rect final : public CGameObject
{
public:
	typedef struct _tagPicking_RectDesc {
		_float3		vPos;			// 사각형의 중점
		_float2		fSize;			// 사각형의 가로/세로 길이
	}Picking_RectDESC;

private:
	CPicking_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CPicking_Rect(const CPicking_Rect& rhs);
	virtual ~CPicking_Rect() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT SetUp_Components(void* pArg);
	HRESULT SetUp_ConstantTable();

private:
	CTexture*			m_pTextureCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CShader*			m_pShaderCom = nullptr;
	CVIBuffer_Rect*		m_pVIBufferCom = nullptr;

public:
	_float3*	Get_RectPos(void) { return m_RectPos; }
	void		Change_Height(_float fHeight);

private:
	_float3				m_RectPos[4];	// 피킹을 위한 점 4개의 정보를 담고 있는 배열, 각 0,1,2 / 0,2,3 의 순서로 삼각형을 만든다.
	_float3				m_vCenterPos;
	_uint				m_iSizeX;
	_uint				m_iSizeY;

public:
	static CPicking_Rect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END