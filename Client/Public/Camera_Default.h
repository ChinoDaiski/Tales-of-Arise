#pragma once

#include "Client_Defines.h"
#include "Camera.h"
#include "Player_Manager.h"
#include "Battle_Manager.h"
BEGIN(Engine)
class CGameInstance;

END


BEGIN(Client)

class CCamera_Default final : public CCamera
{
public:
	enum CAMERA_STATE { CAMERA_STATE_FIELD, CAMERA_STATE_BATTLE, CAMERA_STATE_BATTLE_ENTER, CAMERA_STATE_BATTLE_END, CAMERA_STATE_CHANGE, CAMERA_STATE_END };

private:
	explicit CCamera_Default(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	explicit CCamera_Default(const CCamera_Default& rhs);
	virtual ~CCamera_Default() = default;

public:
	virtual HRESULT NativeConstruct_Prototype();
	virtual HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();


public:
	/*	구면좌표계	*/
	void SphericalCoordinates(_fvector vCamerapos, _fvector vTargetPos);
	void SphericalCoordinatesRotate(_float newAzimuth, _float newElevation);
	void SphericalCoordinatesRotateFixed(_float newAzimuth, _float newElevation);
	void SphericalCoordinatesTranslateRadius(_float newRadius);
	_vector toCartesian();
	void CartesianToLocal(_fvector vTargetPos);



public:
	/*  카메라 쉐이크 */
	void Set_CommonShake(_bool bCommonShake) { m_bCommonShake = bCommonShake; }
	void Set_DownShake(_bool bDownShake) { m_bDownShake = bDownShake; }


public:
	/* 게임환경에 따른 카메라필드 */
	void Camera_Field(_double TimeDelta);
	void Camera_Battle(_double TimeDelta);
	void Camera_BattleEnter(_double TimeDelta);
	void Camera_Change(_double TimeDelta);
	void Set_CameraState(CAMERA_STATE eCameraState) { m_eCameraState = eCameraState; }



public:


	//플레이어 관련
	void Set_PlayerIndex(_uint iIndex) { m_iPlayerIndex = iIndex; }
	void TagetChange();
	void Set_Change(_bool bChange) { m_bChange = bChange; }



public:
	//새로운카메라 도전
	_vector ComputeCameraPos();
	void Follow_Camera(_double TimeDelta);
	void Set_HorzDist(_float dist) { m_fHorzDist = dist; }
	void Set_VertDist(_float dist) { m_fVertDist = dist; }
	void Set_TargetDist(_float dist) { m_fTargetDist = dist; }
	void Set_SpringConstant(_float spring) { m_fSpringConstant = spring; }
	void SnapToIdeal();
	void SnapToIdealAt();
	void SnapToBattleStart();

	//공전 카메라





public:
	//전투관련
	void Set_PreTargetPos(_fvector vPos) { XMStoreFloat3(&m_vPreTargetPos, vPos); }


	//체인지관련
	void Set_NewTarget(_bool bNewTarget) { m_bNewTarget = bNewTarget; }

public:
	static CCamera_Default* Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CAMERA_STATE m_eCameraState = CAMERA_STATE_FIELD;

	_float m_fRadius = 0.f;  //구면좌표계 반지름
	_float m_fAzimuth = 0.f; //방위각 x
	_float m_fElevation = 0.f; //앙각 y
	_float m_fT = 0.f;			//계수
	_float m_minRadius = 0.f;
	_float m_maxRadius = 0.f;
	_float m_minAzimuth = 0.f;
	_float m_RadMinAzimuth = 0.f;
	_float m_maxAzimuth = 0.f;
	_float m_RadMaxAzimuth = 0.f;
	_float m_minElevation = 0.f;
	_float m_RadminElevation = 0.f;
	_float m_maxElevation = 0.f;
	_float m_RadmaxElevation = 0.f;
	_bool m_bSpherical = true;		//구면좌표계 쓸것인지
	_bool m_bSphericalStart = false; //구면좌표계 계수 한번 구해야해서 처음에 호출
	_float m_fCameraLookatPlayerHeight = 0.f;
	_float m_fMouseSensitivity = 0.f; //마우스 감도조절



	_bool m_bChange = false;
	_bool m_bStartScene = false;




	CPlayer_Manager* m_pPlayer_Manager = nullptr;
	CBattle_Manager* m_pBattle_Manager = nullptr;

	CGameInstance* m_pGameInstance = nullptr;

	_uint m_iPlayerIndex = 0;





	//카메라 쉐이킹
	_double m_CommonShakeTime = 0.0;
	_double m_DownShakeTime = 0.0;
	_float m_fShakeDown = 0.f;

	_bool m_bCommonShake = false;
	_bool m_bDownShake = false;





	//카메라 선형보관
	_float3 m_vPrePlayerPos;
	_float3 m_vCurPlayerPos;
	_float3 m_vPreTargetPos;
	_float3 m_vCurTargetPos;


	_float m_fPlayerPosLerp = 0.f;
	_float m_fTargetPosLerp = 0.f;

	_float m_fAtLerp = 0.f;
	_float m_fEyeLerp = 0.f;



	_float3 m_vPreCameraAt;
	_float3 m_vPreCameraEye;
	_float3 m_vCurCameraAt;
	_float3 m_vCurCameraEye;
	_float3 m_vCameraAt;
	_float3 m_vCameraEye;




	//카메라 배틀 관련
	_bool m_bBattleStart = false;




	//카메라 체인지 관련
	_bool m_bNewTarget = false;
	_float m_fCameraChangeAtLerp = 0.f;
	_float m_fCameraChangeEyeLerp = 0.f;
	_float3 m_vCameraChangeStartAt;
	_float3 m_vCameraChangeStartEye;

	//새로운 카메라


	// 카메라의 실제 위치
	_float3 m_vActualPos;
	// 실제 카메라의 속도
	_float3 m_vVelocity;

	// 카메라의 At 실제 위치
	_float3 m_vActualAt;
	// 실제 카메라의 At 속도
	_float3 m_vVelocityAt;

	// 수평거리
	_float m_fHorzDist = 3.5f;
	// 수직거리
	_float m_fVertDist = 1.5f;
	// 타겟까지 거리
	_float m_fTargetDist = 5.f;
	// 스프링 상수(높을 수록 복원력이 강하다)
	_float m_fSpringConstant = 64.f;

	// 스프링 상수(높을 수록 복원력이 강하다)
	_float m_fSpringConstantAt = 32.f;



	//공전 카메라
	_float3 m_vOffSet;
	_float3 m_vUp = _float3(0.f, 1.f, 0.f);
	_float m_fPitchSpeed = 0.f;
	_float m_fYawSpeed = 0.f;

	_bool m_bInitializeSpherical = false;


};

END