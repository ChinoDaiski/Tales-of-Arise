#include "stdafx.h"
#include "..\Public\Camera_Default.h"
#include "GameInstance.h"
#include"Player.h"
#include "Input_Device.h"
CCamera_Default::CCamera_Default(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CCamera(pDeviceOut, pDeviceContextOut)
	, m_pPlayer_Manager(CPlayer_Manager::GetInstance())
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

CCamera_Default::CCamera_Default(const CCamera_Default & rhs)
	: CCamera(rhs)
	, m_pPlayer_Manager(CPlayer_Manager::GetInstance())
	, m_pGameInstance(CGameInstance::GetInstance())
{
}

HRESULT CCamera_Default::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Default::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 10.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	//구면좌표계 관련
	m_fCameraLookatPlayerHeight = 0.4f;	//카메라가 플레이어 높이 어디 처다볼지
	m_fMouseSensitivity = 0.3f;			//마우스 민감도
	m_fRadius = 6.f;
	m_minRadius = 1.5f;
	m_maxRadius = 10.f;
	m_minAzimuth = 0.f;
	m_maxAzimuth = 360.f;

	m_minElevation = 0.f;
	m_maxElevation = 70.f;

	m_pPlayer_Manager->Set_Camera(this);

	return S_OK;
}

void CCamera_Default::Tick(_double TimeDelta)
{

	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	_vector vPlayerPos = ((CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"), m_pPlayer_Manager->Get_MainPlayerIndex()))->Get_State(CTransform::STATE_POSITION);

	if (!m_bSphericalStart)
	{

		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_bSphericalStart = true;
		SphericalCoordinates(vPos, vPlayerPos);
		m_fAzimuth = XMConvertToRadians(-90.f);   //
		m_fElevation = XMConvertToRadians(38.5f);
		//m_fRadius = 6.f;
		m_fRadius = 6.f;
		_vector vCameraPos = toCartesian() + vPlayerPos;
		XMStoreFloat3(&m_vPrePlayerPos, vPlayerPos);
		XMStoreFloat3(&m_vCurPlayerPos, vPlayerPos);

		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));
		XMStoreFloat3(&m_vCameraEye, vPlayerPos);
		m_pTransformCom->LookAt(XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));
		XMStoreFloat3(&m_vCameraAt, XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));
	}


	//F1키 로 구면좌표계<->선생님카메라
	if (pGameInstance->Key_Down(DIK_F1))
	{
		if (m_bSpherical)
			m_bSpherical = false;
		else
			m_bSpherical = true;
	}


	switch (m_eCameraState)
	{
	case Client::CCamera_Default::CAMERA_STATE_FIELD:
		Camera_Field(TimeDelta);
		break;
	case Client::CCamera_Default::CAMERA_STATE_BATTLE_ENTER:
		Camera_BattleEnter(TimeDelta);
		break;
	case Client::CCamera_Default::CAMERA_STATE_BATTLE:
		Follow_Camera(TimeDelta);
		break;
	case Client::CCamera_Default::CAMERA_STATE_CHANGE:
		Camera_Change(TimeDelta);
		break;
	case Client::CCamera_Default::CAMERA_STATE_END:
		break;
	default:
		break;
	}







	//_float fSpeed = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	//m_fPlayerPosLerp += TimeDelta *pow(fSpeed, 1.5);
	//if (m_fPlayerPosLerp >= 1.f)
	//{
	//	m_fPlayerPosLerp = 0.f;
	//	m_vPrePlayerPos = m_vCurPlayerPos;   //전프레임의 알펜 저장 
	//	XMStoreFloat3(&m_vCurPlayerPos, vPlayerPos); //현재 프레임의 알펜 위치저장 
	//

	//}
	//_vector vPlayerLerpPos = XMVectorLerp(XMLoadFloat3(&m_vPrePlayerPos), XMLoadFloat3(&m_vCurPlayerPos), m_fPlayerPosLerp);

	//두 플레이어 포스와 나사이를 선형보간 .


	//if (m_pBattle_Manager->Get_Battle())
	//{
	// 

	// _float fSpeed = XMVectorGetX(XMVector3Length(m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos() - XMLoadFloat3(&m_vCameraAt)));
	// m_fTargetPosLerp += TimeDelta *pow(fSpeed, 1.5);
	// if (m_fPlayerPosLerp >= 1.f)
	// {
	//	 m_fPlayerPosLerp = 0.f;
	//	 m_vPreTargetPos = m_vCurTargetPos;   //전프레임의 알펜 저장 
	//	 XMStoreFloat3(&m_vCurPlayerPos, vPlayerPos); //현재 프레임의 알펜 위치저장 

	// }


	// m_pTransformCom->LookAt(XMVectorLerp(XMLoadFloat3(&m_vPrePlayerPos), XMLoadFloat3(&m_vCurPlayerPos), m_fPlayerPosLerp));

	// XMStoreFloat3(&m_vCurTargetPos, m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos());
	//}



	if (m_bChange)
	{

		//m_pTransformCom->LookAt(m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos());
	}
	else {

		if (m_bSpherical) //구면좌표계	
		{

		}
		else
		{
			if (pGameInstance->Get_DIKeyState(DIK_W) & 0x8000)
			{
				m_pTransformCom->Go_Straight(TimeDelta);
			}
			if (GetKeyState('S') & 0x8000)
			{
				m_pTransformCom->Go_BackWard(TimeDelta);
			}
			if (GetKeyState('A') & 0x8000)
			{
				m_pTransformCom->Go_Left(TimeDelta);
			}
			if (GetKeyState('D') & 0x8000)
			{
				m_pTransformCom->Go_Right(TimeDelta);
			}



			_long		MouseMove;

			if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
			{
				m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.1f);
			}

			if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
			{
				m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove* 0.1f);
			}

		}
	}




	//카메라흔들기

	if (m_bDownShake)
	{
		m_DownShakeTime += TimeDelta;
		if (m_DownShakeTime > 0.25)
		{
			m_DownShakeTime = 0.0;
			m_bDownShake = false;
			m_fShakeDown = 0.f;
		}

		if (m_DownShakeTime < 0.125)
		{
			m_fShakeDown += (rand() % 500) / 2500.f;
		}
		else {
			m_fShakeDown -= (rand() % 500) / 2500.f;
		}


		/*  아래쪽으로 점점 흔들리는 카메라. */


		_float x = (rand() % 1000 - 500) / 2500.f;
		_vector vCameraShake = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))*-m_fShakeDown + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT))*x;
		_vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + vCameraShake);


	}

	if (m_bCommonShake)
	{

		m_CommonShakeTime += TimeDelta;
		if (m_CommonShakeTime > 0.1)
		{
			m_CommonShakeTime = 0.0;
			m_bCommonShake = false;
		}


		_float x = (rand() % 1000 - 500) / 10000.f;
		_float y = (rand() % 1000 - 500) / 10000.f;
		_float z = (rand() % 1000 - 500) / 10000.f;
		_vector vCameraShake = XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_UP))*y + XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT))*x;
		_vector vCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos + vCameraShake);
	}





	Safe_Release(pGameInstance);
}

void CCamera_Default::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CCamera_Default::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_Default::SphericalCoordinates(_fvector vCamerapos, _fvector vTargetPos)
{
	m_RadMinAzimuth = XMConvertToRadians(m_minAzimuth);
	m_RadMaxAzimuth = XMConvertToRadians(m_maxAzimuth);

	m_RadmaxElevation = XMConvertToRadians(m_maxElevation);
	m_RadminElevation = XMConvertToRadians(m_minElevation);

	m_fAzimuth = atan2f(XMVectorGetZ(vCamerapos), XMVectorGetX(vCamerapos));
	m_fElevation = asinf(XMVectorGetY(vCamerapos) / m_fRadius);

	m_fT = m_fRadius*cosf(m_fElevation);

	_vector vCameraPos = XMVectorSet(m_fT*cosf(m_fAzimuth), m_fRadius*sinf(m_fElevation), m_fT*sinf(m_fAzimuth), 1.f);
	vCameraPos += vTargetPos;
	XMVectorSetW(vCameraPos, 1.f);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vCameraPos);

}

void CCamera_Default::SphericalCoordinatesRotate(_float newAzimuth, _float newElevation)
{
	m_fAzimuth += newAzimuth;
	m_fElevation += newElevation;

	//repeat
	m_fAzimuth = m_fAzimuth - ((_uint)(m_fAzimuth / 360.f))*360.f;

	//clamp
	if (m_fElevation >= m_RadmaxElevation)
	{
		m_fElevation = m_RadmaxElevation;
	}
	else if (m_fElevation <= m_RadminElevation)
	{
		m_fElevation = m_RadminElevation;
	}
}

void CCamera_Default::SphericalCoordinatesTranslateRadius(_float newRadius)
{
	m_fRadius += newRadius;

	if (m_fRadius >= m_maxRadius)
	{
		m_fRadius = m_maxRadius;
	}
	else if (m_fRadius <= m_minRadius)
	{
		m_fRadius = m_minRadius;
	}
}

_vector CCamera_Default::toCartesian()
{

	m_fT = m_fRadius*cosf(m_fElevation);
	_vector vCameraPos = XMVectorSet(m_fT*cosf(m_fAzimuth), m_fRadius*sinf(m_fElevation), m_fT*sinf(m_fAzimuth), 1.f);
	vCameraPos = XMVectorSetW(vCameraPos, 1.f);

	return vCameraPos;

}

void CCamera_Default::CartesianToLocal(_fvector vTargetPos)
{

	_float t, e, a;



	_float posy = XMVectorGetY(vTargetPos);
	_float posx = XMVectorGetX(vTargetPos);

	e = asinf(posy / m_fRadius);
	t = m_fRadius*cosf(e);
	a = acosf(posx / t);

	m_fT = t;
	m_fAzimuth = a;
	m_fElevation = e;



}


void CCamera_Default::Camera_Field(_double TimeDelta)
{

	_long MouseMoveX = 0, MouseMoveY = 0, MouseMoveWheel = 0; //방위각,앙각 
	_vector vCameraPos;

	_vector vPlayerPos = m_pPlayer_Manager->Get_FieldPlayer()->Get_PlayerPos();


	if (MouseMoveX = -m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
	{


	}
	if (MouseMoveY = m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
	{

	}
	if (MouseMoveWheel = -m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_WHEEL))
	{

	}
	SphericalCoordinatesTranslateRadius((_float)MouseMoveWheel*1.2f*(_float)TimeDelta*0.2f);
	SphericalCoordinatesRotate((_float)MouseMoveX*(_float)TimeDelta*m_fMouseSensitivity, (_float)MouseMoveY*(_float)TimeDelta*m_fMouseSensitivity);
	vCameraPos = toCartesian() + vPlayerPos;
	XMStoreFloat3(&m_vCameraEye, vCameraPos);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));
	m_pTransformCom->LookAt(XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));
	XMStoreFloat3(&m_vCameraAt, XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));

	//else {
	//	//_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
	//	vCameraPos = toCartesian() + vPlayerPos;
	//	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));
	//	m_pTransformCom->LookAt(XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));
	//	XMStoreFloat3(&m_vCameraAt, XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));

	//}

}

void CCamera_Default::Camera_Battle(_double TimeDelta)
{

	_long MouseMoveX = 0, MouseMoveY = 0, MouseMoveWheel = 0; //방위각,앙각 
	_vector vCameraPos;

	_vector vPlayerPos = m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos();
	_vector vTargetPos = m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos();

	_vector vDir = vTargetPos - vPlayerPos;  // 실제 타깃과 플레이어와의 방향 (노말라이즈하지 않으면 실제 거리값을 가지고있는다);
	_vector vOriginDir = vDir;
	_float fDistance = XMVectorGetX(XMVector3Length(vDir));

	vDir = XMVectorSetY(vDir, 0.f);
	// 이거리 값을 이용해 실제 카메라의 반지름 을 구한다. (타깃이 멀리있으면 반지름을 멀리 가까이있으면  가깝게.




	if (!m_bBattleStart)
	{
		m_vPreCameraAt = m_vCameraAt;
		m_vPreCameraEye = m_vCameraEye;
		m_vCurCameraAt = m_vCameraAt;
		m_vCurCameraEye = m_vCameraEye;

		////lerp가 100다될떄까지 
		//
		//_vector vPreAt = XMLoadFloat3(&m_vCameraAt);  //이전 카메라의 at;
		//_vector vPreEye = XMLoadFloat3(&m_vCameraEye);  //이전 카메라의 eye;
		//

		m_bBattleStart = true;
	}

	_float fSpeed = XMVectorGetX(XMVector3Length(m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos() - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));



	m_fEyeLerp += TimeDelta *pow(fSpeed, 1.5)*0.4f;
	if (m_fEyeLerp >= 1.f)
	{
		m_fEyeLerp = 0.f;
		m_vPreCameraEye = m_vCurCameraEye;
		XMStoreFloat3(&m_vCurCameraEye, vPlayerPos - XMVector3Normalize(vDir)*4.f + XMVectorSet(0.f, 1.f, 0.f, 1.f)* m_fCameraLookatPlayerHeight*4.f);//현재 프레임의 알펜 위치저장 

	}




	//fSpeed = XMVectorGetX(XMVector3Length(m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos() - XMLoadFloat3(&m_vCameraAt)));

	m_fAtLerp += TimeDelta *pow(fSpeed, 1.5)*0.4f;
	if (m_fAtLerp >= 1.f)
	{
		m_fAtLerp = 0.f;
		m_vPreCameraAt = m_vCurCameraAt;
		XMStoreFloat3(&m_vCurCameraAt, vPlayerPos + vDir*0.2f); //현재 프레임의 알펜 위치저장 

	}







	//_float fSpeed = XMVectorGetX(XMVector3Length(vPlayerPos - m_pTransformCom->Get_State(CTransform::STATE_POSITION)));
	//m_fPlayerPosLerp += TimeDelta *pow(fSpeed, 1.5);
	//if (m_fPlayerPosLerp >= 1.f)
	//{
	// m_fPlayerPosLerp = 0.f;
	// m_vPrePlayerPos = m_vCurPlayerPos;   //전프레임의 알펜 저장 
	// XMStoreFloat3(&m_vCurPlayerPos, vPlayerPos); //현재 프레임의 알펜 위치저장 

	//}



	if (m_pGameInstance->Key_Pressing(CInput_Device::DIMB_RBUTTON))
	{

		if (MouseMoveX = -m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
		{


		}
		if (MouseMoveY = m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
		{

		}
		if (MouseMoveWheel = -m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_WHEEL))
		{

		}
		SphericalCoordinatesTranslateRadius((_float)MouseMoveWheel*1.2f*(_float)TimeDelta*0.2f);
		SphericalCoordinatesRotate((_float)MouseMoveX*(_float)TimeDelta*m_fMouseSensitivity, (_float)MouseMoveY*(_float)TimeDelta*m_fMouseSensitivity);

	}

	vCameraPos = /*toCartesian() +*/ XMQuaternionSlerp(XMLoadFloat3(&m_vPreCameraEye), XMLoadFloat3(&m_vCurCameraEye), m_fEyeLerp);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));
	//m_pTransformCom->LookAt(XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vPreCameraAt), XMLoadFloat3(&m_vCurCameraAt), m_fAtLerp),1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMQuaternionSlerp(XMLoadFloat3(&m_vPreCameraAt), XMLoadFloat3(&m_vCurCameraAt), m_fAtLerp), 1.f));

	XMStoreFloat3(&m_vCameraAt, XMQuaternionSlerp(XMLoadFloat3(&m_vPreCameraAt), XMLoadFloat3(&m_vCurCameraAt), m_fAtLerp));

}

void CCamera_Default::Camera_BattleEnter(_double TimeDelta)
{
	if (!m_bStartScene)
	{
		SnapToBattleStart();
		m_vVelocity = _float3(0.f, 0.f, 0.f);
		m_vVelocityAt = _float3(0.f, 0.f, 0.f);
		m_bStartScene = true;
	}

	// 스프링 상숫값으로부터 감쇄 인자값 계산
	_float dampeningAt = 2.0f *sqrtf(m_fSpringConstantAt);

	_vector vPlayerPos = m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos();
	_vector vTargetPos = m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos();

	_vector vDir = vTargetPos - vPlayerPos;  // 실제 타깃과 플레이어와의 방향 (노말라이즈하지 않으면 실제 거리값을 가지고있는다);
	_vector vOriginDir = vDir;
	vDir = XMVectorSetY(vDir, 0.f);

	_vector idealAt = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight);
	_vector diffAt = XMLoadFloat3(&m_vActualAt) - idealAt;
	_vector acelAt = -m_fSpringConstantAt * diffAt - dampeningAt * XMLoadFloat3(&m_vVelocityAt);
	// 속도를 갱신
	XMStoreFloat3(&m_vVelocityAt, XMLoadFloat3(&m_vVelocityAt) + acelAt * TimeDelta);
	// 실제 카메라의 위치를 갱신
	XMStoreFloat3(&m_vActualAt, XMLoadFloat3(&m_vActualAt) + XMLoadFloat3(&m_vVelocityAt) * TimeDelta);

	_float dampening = 2.0f *sqrtf(m_fSpringConstantAt);

	_float fDistance = XMVectorGetX(XMVector3Length(vDir));







	// 이상적인 위치 계산
	_vector idealPos = ComputeCameraPos();
	// 이상적인 위치와 실제 위치의 차를 계산
	_vector diff = XMLoadFloat3(&m_vActualPos) - idealPos;
	// 스프링의 가속도를 계산한다.
	_vector acel = -m_fSpringConstant * diff - dampening * XMLoadFloat3(&m_vVelocity);

	if (XMVectorGetX(XMVector3Length(diff)) < 0.15f)
	{
		m_eCameraState = CAMERA_STATE_BATTLE;
		m_bStartScene = false;
		return;
	}

	// 속도를 갱신
	XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + acel * TimeDelta);
	// 실제 카메라의 위치를 갱신
	XMStoreFloat3(&m_vActualPos, XMLoadFloat3(&m_vActualPos) + XMLoadFloat3(&m_vVelocity) * TimeDelta);



	// 타깃은 소유자 앞에 있는 대상을 의미
	_vector  vtarget = m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos() + m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerLook() * m_fTargetDist;
	// 카메라가 뒤집혀지는 경우는 없기에 위쪽 방향은 항상 UnitZ이다
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vActualPos), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vActualAt), 1.f));

}

void CCamera_Default::Camera_Change(_double TimeDelta)
{


	_vector vCameraPos;

	_vector vPlayerPos = m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos();
	_vector vTargetPos = m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos();


	if (!m_bNewTarget)
	{
		m_bNewTarget = true;
		m_fCameraChangeAtLerp = 0.f;
		m_fCameraChangeEyeLerp = 0.f;
		m_vCameraChangeStartAt = m_vActualAt;
		m_vCameraChangeStartEye = m_vActualPos;
	}

	m_fCameraChangeAtLerp += TimeDelta*2.f;
	m_fCameraChangeEyeLerp += TimeDelta*2.f;

	if (m_fCameraChangeAtLerp >= 1.f)
		m_fCameraChangeAtLerp = 1.f;

	if (m_fCameraChangeEyeLerp >= 1.f)
		m_fCameraChangeEyeLerp = 1.f;

	_vector vTargetEye = vTargetPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f) - XMVector3Normalize(vTargetPos - XMLoadFloat3(&m_vCameraChangeStartEye))*5.f;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vCameraChangeStartEye), vTargetEye, m_fCameraChangeEyeLerp), 1.f));
	m_pTransformCom->LookAt(XMVectorSetW(XMVectorLerp(XMLoadFloat3(&m_vCameraChangeStartAt), vTargetPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f), m_fCameraChangeAtLerp), 1.f));

	XMStoreFloat3(&m_vActualPos, XMVectorLerp(XMLoadFloat3(&m_vCameraChangeStartEye), vTargetEye, m_fCameraChangeEyeLerp));
	XMStoreFloat3(&m_vActualAt, XMVectorLerp(XMLoadFloat3(&m_vCameraChangeStartAt), vTargetPos + XMVectorSet(0.f, 0.5f, 0.f, 0.f), m_fCameraChangeAtLerp));



}

void CCamera_Default::TagetChange()
{




}

_vector CCamera_Default::ComputeCameraPos()
{

	CPlayer* pMainPlayer = m_pPlayer_Manager->Get_MainPlayer();
	_vector vPlayerPos = pMainPlayer->Get_PlayerPos();
	_vector vTargetPos = pMainPlayer->Get_TargetPos();

	_vector vDir = vTargetPos - vPlayerPos;  // 실제 타깃과 플레이어와의 방향 (노말라이즈하지 않으면 실제 거리값을 가지고있는다);
	_vector vOriginDir = vDir;
	vDir = XMVectorSetY(vDir, 0.f);

	_vector cameraPos = pMainPlayer->Get_PlayerPos();
	cameraPos -= XMVector3Normalize(vDir) * m_fHorzDist*2.f;
	cameraPos += XMVectorSet(0.f, 1.f, 0.f, 0.f)* m_fVertDist*1.5f;
	return cameraPos;
}

void CCamera_Default::Follow_Camera(_double TimeDelta)
{



	_long MouseMoveX = 0, MouseMoveY = 0, MouseMoveWheel = 0; //방위각,앙각 
	_vector vCameraPos;

	_vector vPlayerPos = m_pPlayer_Manager->Get_FieldPlayer()->Get_PlayerPos();


	if (m_pGameInstance->Button_Pressing(CInput_Device::DIMB_RBUTTON))
	{
		if (!m_bInitializeSpherical)
		{
			_vector vLocalCameraPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vPlayerPos; //로컬상의 카메라좌표
			m_fRadius = XMVectorGetX(XMVector3Length(vLocalCameraPos));
			m_fAzimuth = atan2f(XMVectorGetZ(vLocalCameraPos), XMVectorGetX(vLocalCameraPos));
			m_fElevation = asinf(XMVectorGetY(vLocalCameraPos) / m_fRadius);
			m_bInitializeSpherical = true;
		}
		if (MouseMoveX = -m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_X))
		{


		}
		if (MouseMoveY = m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
		{

		}
		if (MouseMoveWheel = -m_pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_WHEEL))
		{

		}




		SphericalCoordinatesTranslateRadius((_float)MouseMoveWheel*1.2f*(_float)TimeDelta*0.2f);
		SphericalCoordinatesRotate((_float)MouseMoveX*(_float)TimeDelta*m_fMouseSensitivity, (_float)MouseMoveY*(_float)TimeDelta*m_fMouseSensitivity);
		vCameraPos = toCartesian() + vPlayerPos;
		XMStoreFloat3(&m_vActualPos, vCameraPos);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vCameraPos, 1.f));
		m_pTransformCom->LookAt(XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));
		XMStoreFloat3(&m_vActualAt, XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight));






	}

	else {
		m_bInitializeSpherical = false;





		if (!m_bBattleStart)
		{
			m_bBattleStart = true;
			SnapToIdeal();

			m_vPreCameraAt = m_vCameraAt;
			m_vPreCameraEye = m_vCameraEye;
			m_vCurCameraAt = m_vCameraAt;
			m_vCurCameraEye = m_vCameraEye;
		}

		///////////////////////////////// At/////////////////////////////////



		// 스프링 상숫값으로부터 감쇄 인자값 계산
		_float dampeningAt = 2.0f *sqrtf(m_fSpringConstantAt);

		_vector vPlayerPos = m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos();
		_vector vTargetPos = m_pPlayer_Manager->Get_MainPlayer()->Get_TargetPos();

		//if (acosf( XMVectorGetX(XMVector3Dot(XMVector3Normalize(XMVectorSetY(vTargetPos - vPlayerPos, 0.f)), XMVector3Normalize(XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_LOOK), 0.f)))))>XMConvertToRadians(20.f))
		//{
		_vector vDir = vTargetPos - vPlayerPos;  // 실제 타깃과 플레이어와의 방향 (노말라이즈하지 않으면 실제 거리값을 가지고있는다);
		_vector vOriginDir = vDir;
		vDir = XMVectorSetY(vDir, 0.f);
		_float fDistance = XMVectorGetX(XMVector3Length(vDir));
		_vector idealAt = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight);  //vPlayerPos + vDir*0.5f;
		_vector diffAt = XMLoadFloat3(&m_vActualAt) - idealAt;
		_vector acelAt = -m_fSpringConstantAt * diffAt - dampeningAt * XMLoadFloat3(&m_vVelocityAt);
		// 속도를 갱신
		XMStoreFloat3(&m_vVelocityAt, XMLoadFloat3(&m_vVelocityAt) + acelAt * TimeDelta);
		// 실제 카메라의 위치를 갱신
		XMStoreFloat3(&m_vActualAt, XMLoadFloat3(&m_vActualAt) + XMLoadFloat3(&m_vVelocityAt) * TimeDelta);

		//}






		//////////////////////////////////EYE////////////////////////////

		_float dampening = 2.0f *sqrtf(m_fSpringConstant);
		// 이상적인 위치 계산
		_vector idealPos = ComputeCameraPos();
		// 이상적인 위치와 실제 위치의 차를 계산
		_vector diff = XMLoadFloat3(&m_vActualPos) - idealPos;
		// 스프링의 가속도를 계산한다.
		_vector acel = -m_fSpringConstant * diff - dampening * XMLoadFloat3(&m_vVelocity);

		// 속도를 갱신
		XMStoreFloat3(&m_vVelocity, XMLoadFloat3(&m_vVelocity) + acel * TimeDelta);
		// 실제 카메라의 위치를 갱신
		XMStoreFloat3(&m_vActualPos, XMLoadFloat3(&m_vActualPos) + XMLoadFloat3(&m_vVelocity) * TimeDelta);



		// 타깃은 소유자 앞에 있는 대상을 의미
		_vector  vtarget = m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerPos() + m_pPlayer_Manager->Get_MainPlayer()->Get_PlayerLook() * m_fTargetDist;
		// 카메라가 뒤집혀지는 경우는 없기에 위쪽 방향은 항상 UnitZ이다
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vActualPos), 1.f));
		m_pTransformCom->LookAt(XMVectorSetW(XMLoadFloat3(&m_vActualAt), 1.f));

	}











}

void CCamera_Default::SnapToIdeal()
{
	CPlayer* pMainPlayer = m_pPlayer_Manager->Get_MainPlayer();
	_vector vPlayerPos = pMainPlayer->Get_PlayerPos();
	_vector vTargetPos = pMainPlayer->Get_TargetPos();

	_vector vDir = vTargetPos - vPlayerPos;  // 실제 타깃과 플레이어와의 방향 (노말라이즈하지 않으면 실제 거리값을 가지고있는다);
	_vector vOriginDir = vDir;
	vDir = XMVectorSetY(vDir, 0.f);
	_vector idealAt = XMVectorSetY(vPlayerPos, XMVectorGetY(vPlayerPos) + m_fCameraLookatPlayerHeight);

	m_fSpringConstant = 256.f;


	// 실제 위치를 이상적인 위치로 설정
	XMStoreFloat3(&m_vActualPos, ComputeCameraPos());
	XMStoreFloat3(&m_vActualAt, idealAt);
	// 속도를 0으로 초기화
	m_vVelocity = _float3(0.f, 0.f, 0.f);
	m_vVelocityAt = _float3(0.f, 0.f, 0.f);
	// 타깃 지점과 뷰 행렬을 계산
	_vector  vtarget = pMainPlayer->Get_PlayerPos() + pMainPlayer->Get_PlayerLook() * m_fTargetDist;
	//	_matrix CameraWolrdMatrix =XMMatrixLookAtLH(XMLoadFloat3(&m_vActualPos), vtarget, XMVectorSet(0.f, 0.f, 1.f, 0.f));

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(XMLoadFloat3(&m_vActualPos), 1.f));
	m_pTransformCom->LookAt(vPlayerPos);

	//m_pTransformCom->Set_WorldMatrix(CameraWolrdMatrix);
}

void CCamera_Default::SnapToIdealAt()
{
}

void CCamera_Default::SnapToBattleStart()
{

	CPlayer* pMainPlayer = m_pPlayer_Manager->Get_MainPlayer();
	_vector vPlayerPos = pMainPlayer->Get_PlayerPos();
	_vector vPlayerLook = pMainPlayer->Get_PlayerLook();
	m_fSpringConstant = 16.f;


	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPlayerPos - vPlayerLook + XMVectorSet(0.f, 0.4f, 0.f, 0.f));

	XMStoreFloat3(&m_vActualPos, vPlayerPos - vPlayerLook + XMVectorSet(0.f, 0.4f, 0.f, 0.f));

	m_pTransformCom->LookAt(vPlayerPos + XMVectorSet(0.f, 0.1f, 0.f, 0.f));
	XMStoreFloat3(&m_vActualAt, vPlayerPos + XMVectorSet(0.f, 0.1f, 0.f, 0.f));


}

CCamera_Default * CCamera_Default::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCamera_Default*	pInstance = new CCamera_Default(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_Default"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_Default::Clone(void * pArg)
{
	CCamera_Default*	pInstance = new CCamera_Default(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_Default"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Default::Free()
{
	__super::Free();

}