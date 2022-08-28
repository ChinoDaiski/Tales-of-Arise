#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Engine)

class CTransform;

END


BEGIN(Client)

class CCamera_MapTool final : public CCamera
{
private:
	explicit CCamera_MapTool(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CCamera_MapTool(const CCamera_MapTool& rhs);
	virtual ~CCamera_MapTool() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();


public:
	void Adjust_Angle(_float fDegree);
	void Rotation(_float fDegree);
	void Cal_CurEyeAt(void);
	void Cal_Distance(void);
	void Cal_Degree(void);
	void SetPos(_fvector _Eye, _fvector _At, _fvector _Up);
	void Press_Key(_double TimeDelta);

public:
	_float Get_DegreeUD(void) { return m_fDegreeUD; }
	void Set_DegreeUD(_float fDegree) { m_fDegreeUD = fDegree; }

	_float Get_DegreeLR(void) { return m_fDegreeLR; }
	void Set_DegreeLR(_float fDegree) { m_fDegreeLR = fDegree; }

private:
	_float3		m_CurEye;		// 카메라의 현재 Eye
	_float3		m_CurAt;		// 카메라의 현재 At

	_float		m_fDegreeUD = 0.f;		// 카메라의 각도(상하)
	_float		m_fDegreeLR = 0.f;		// 카메라의 각도(좌우)
	_float		m_fDistance = 1.f;		// 카메라의 eye와 at 사이의 거리

	_bool		m_bChangeMove = false;	// 카메라의 움직임을 tool로 제어하는지 안하는지에 대한 변수, true면 마우스로 움직이고, false면 툴로 움직인다.

public:
	static CCamera_MapTool* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END