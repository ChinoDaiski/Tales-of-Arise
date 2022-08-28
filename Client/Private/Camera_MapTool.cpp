#include "stdafx.h"
#include "..\Public\Camera_MapTool.h"
#include "GameInstance.h"

CCamera_MapTool::CCamera_MapTool(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CCamera(pDeviceOut, pDeviceContextOut)
{
}

CCamera_MapTool::CCamera_MapTool(const CCamera_MapTool & rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_MapTool::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_MapTool::NativeConstruct(void * pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_CameraDesc, pArg, sizeof(CAMERADESC));

		m_CurEye = m_CameraDesc.vEye;
		m_CurAt = m_CameraDesc.vAt;
	}

	return S_OK;
}

void CCamera_MapTool::Tick(_double TimeDelta)
{
	// Eye At값을 수정한다.
	Cal_CurEyeAt();

	// 거리 값 설정하기
	Cal_Distance();

	// 각도 설정하기
	Cal_Degree();


	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	if (pGameInstance->Key_Pressing(DIK_LSHIFT)) {

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
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * 0.003f);
		}

		if (MouseMove = pGameInstance->Get_DIMMoveState(CInput_Device::DIMM_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove* 0.003f);
		}
	}
	else {
		// 키 처리
		Press_Key(TimeDelta);
	}

	__super::Tick(TimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

void CCamera_MapTool::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
}

HRESULT CCamera_MapTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CCamera_MapTool::Adjust_Angle(_float fDegree)
{
	// 카메라의 Angle 값을 조정하고, 해당 값으로 카메라의 at 값을 조정한다.
	int iAngle = (int)fDegree;
	m_fDegreeUD = (_float)iAngle;

	// 바뀐 각도값을 사용하여 카메라의 at값을 조정한다.

	// 원점에서 m_fDistance만큼 떨어진 위치에 있는 점을 만든다.
	Cal_Distance();
	_float3 vPos = _float3(0.f, 0.f, m_fDistance);




	// 해당 점을 x축으로 m_fDegreeUD만큼 돌린다.
	_float4x4 matfloat4x4;
	_matrix mat = XMMatrixIdentity();
	XMStoreFloat4x4(&matfloat4x4, mat);
	memcpy(matfloat4x4.m[3], &vPos, sizeof(_float3));

	mat = XMLoadFloat4x4(&matfloat4x4);
	mat = mat * XMMatrixRotationX(XMConvertToRadians(m_fDegreeUD));
	XMStoreFloat4x4(&matfloat4x4, mat);
	memcpy(&vPos, matfloat4x4.m[3], sizeof(_float3));




	// 해당 점을 y축으로 m_fDegreeLR만큼 돌린다.
	mat = XMMatrixIdentity();
	XMStoreFloat4x4(&matfloat4x4, mat);
	memcpy(matfloat4x4.m[3], &vPos, sizeof(_float3));

	mat = XMLoadFloat4x4(&matfloat4x4);
	mat = mat * XMMatrixRotationY(XMConvertToRadians(m_fDegreeLR));
	XMStoreFloat4x4(&matfloat4x4, mat);
	memcpy(&vPos, matfloat4x4.m[3], sizeof(_float3));




	// 카메라의 eye값을 가져온다.	
	_vector vEye = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	// 카메라의 at값을 구한다.
	_vector vAt = vEye + XMLoadFloat3(&vPos);

	// 카메라의 위치를 다시 설정한다.
	SetPos(vEye, vAt, XMVectorSet(0.f, 1.f, 0.f, 1.f));
}

void CCamera_MapTool::Rotation(_float fDegree)
{
	m_pTransformCom->Rotation(XMVectorSet(0.f, 1.f, 0.f, 1.f), XMConvertToRadians(fDegree));
	m_fDegreeLR = fDegree;
}

void CCamera_MapTool::Cal_CurEyeAt(void)
{
	XMStoreFloat3(&m_CurEye, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	XMStoreFloat3(&m_CurAt, XMVector3Normalize(vLook) + XMLoadFloat3(&m_CurEye));
}

void CCamera_MapTool::Cal_Distance(void)
{
	_vector vEye = XMLoadFloat3(&m_CurEye);
	_vector vAt = XMLoadFloat3(&m_CurAt);

	m_fDistance = XMVectorGetX(XMVector3Length(vAt - vEye));
}

void CCamera_MapTool::Cal_Degree(void)
{
	// look 방향 벡터를 구한다.
	_vector vLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

	// 해당 벡터의 y값만 0으로 하면 xz평면상의 방향 벡터가 나온다.
	_vector vLook_xz = XMVectorSetY(vLook, 0.f);

	// 두 방향벡터의 사이각을 구한다.
	_vector vDot = XMVector3Dot(vLook, vLook_xz);
	_vector fRadin = XMVectorACos(vDot / (XMVector3Length(vLook) * XMVector3Length(vLook_xz)));
	_float fDegree = XMConvertToDegrees(XMVectorGetX(fRadin));

	// 멤버변수에 값을 저장한다
	// 만약 eye의 y값보다 at의 y값이 더 크다면 음수를 곱한다.
	if (m_CurEye.y < m_CurAt.y)
		m_fDegreeUD *= -1.f;

	m_fDegreeUD = fDegree;
}

void CCamera_MapTool::SetPos(_fvector vEye, _fvector vAt, _fvector vUpAxis)
{
	_vector		vLook = vAt - vEye;
	m_pTransformCom->Set_State(CTransform::STATE_LOOK, XMVector3Normalize(vLook));

	_vector		vRight = XMVector3Cross(vUpAxis, vLook);
	m_pTransformCom->Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(vRight));

	_vector		vUp = XMVector3Cross(vLook, vRight);
	m_pTransformCom->Set_State(CTransform::STATE_UP, XMVector3Normalize(vUp));

	_vector		vPosition = XMVectorSetW(vEye, 1.f);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

	XMStoreFloat3(&m_CurEye, vEye);
	XMStoreFloat3(&m_CurAt, vAt);
}

void CCamera_MapTool::Press_Key(_double TimeDelta)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();

	Safe_AddRef(pGameInstance);

	if (pGameInstance->Key_Pressing(DIK_W))
	{
		m_pTransformCom->Go_Straight_PlaneXZ(TimeDelta);
	}
	if (pGameInstance->Key_Pressing(DIK_S))
	{
		m_pTransformCom->Go_BackWard_PlaneXZ(TimeDelta);
	}
	if (pGameInstance->Key_Pressing(DIK_A))
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}
	if (pGameInstance->Key_Pressing(DIK_D))
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}

	if (!pGameInstance->Key_Pressing(DIK_R)) {
		if (pGameInstance->Key_Pressing(DIK_NUMPADPLUS))
		{
			m_pTransformCom->Go_BackWard(TimeDelta);
		}
		if (pGameInstance->Key_Pressing(DIK_NUMPADMINUS))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
		}
	}

	//// +,-키를 눌렀을 경우
	//if (pGameInstance->Get_DIKeyState(DIK_NUMPADPLUS)) {
	//	if (!pGameInstance->Get_DIKeyPreState(DIK_NUMPADPLUS))
	//		Adjust_Angle(true);
	//}
	//if (pGameInstance->Get_DIKeyState(DIK_NUMPADMINUS)) {
	//	if (!pGameInstance->Get_DIKeyPreState(DIK_NUMPADMINUS))
	//		Adjust_Angle(false);
	//}

	Safe_Release(pGameInstance);
}

CCamera_MapTool * CCamera_MapTool::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CCamera_MapTool*	pInstance = new CCamera_MapTool(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_MapTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCamera_MapTool::Clone(void * pArg)
{
	CCamera_MapTool*	pInstance = new CCamera_MapTool(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CCamera_MapTool"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_MapTool::Free()
{
	__super::Free();

}
