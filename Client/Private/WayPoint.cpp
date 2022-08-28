#include "stdafx.h"
#include "..\Public\WayPoint.h"
#include "Player.h"

#include "Level_Loading.h"

CWayPoint::CWayPoint(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWayPoint::CWayPoint(const CWayPoint & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWayPoint::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSG_BOX(L"CWayPoint -> NativeConstruct_Prototype -> NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWayPoint::NativeConstruct(void * pArg, CTransform::TRANSFORMDESC* pTransformDesc)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	return S_OK;
}

void CWayPoint::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

#ifdef _DEBUG
	// 충돌체 업데이트
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG

	Check_TargetPlayer_Collision();
}

void CWayPoint::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

#ifdef _DEBUG
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
#endif // _DEBUG
}

HRESULT CWayPoint::Render()
{
#ifdef _DEBUG
	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CWayPoint::SetUp_Components(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	WayPoint_DESC Desc = *((WayPoint_DESC*)pArg);

	// 타겟 플레이어 설정
	m_pTargetPlayer = Desc.pTargetPlayer;

	// 위치 설정
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Desc.fPos.x, Desc.fPos.y, Desc.fPos.z, 1.f));

	// 다음 레벨 설정
	m_eNextLevel = Desc.eLevel;

	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC         ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_SPHERE */
	ColliderDesc.vPosition = _float3(0.f, 1.0f, 0.f);
	ColliderDesc.fRadius = 3.f;

	// 플레이어와 충돌 체크용 구 생성
	if (FAILED(__super::SetUp_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWayPoint::SetUp_ConstantTable()
{

	return S_OK;
}

HRESULT CWayPoint::Check_TargetPlayer_Collision(void)
{
	if (m_bMove)
		return S_OK;

	if (nullptr == m_pTargetPlayer) {
		MSG_BOX(L"CWayPoint : No Player Detected!");
		return E_FAIL;
	}

	CCollider* pPlayer_Collider = dynamic_cast<CCollider*>(m_pTargetPlayer->Get_Component(TEXT("Com_SPHERE_Interaction")));

	if (nullptr == pPlayer_Collider) {
		MSG_BOX(L"CWayPoint : No Collider Detected!");
		return E_FAIL;
	}

	// 자신의 충돌체와 플레이어의 상호작용 충돌체가 충돌했을 경우
	if (m_pSphereCom->Collision(pPlayer_Collider)) {

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

		switch (m_eNextLevel)
		{
		case Client::LEVEL_TUTORIAL:
			break;
		case Client::LEVEL_LORD_BALSEPH:
			pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_LORD_BALSEPH));
			m_bMove = true;
			break;
		case Client::LEVEL_FIRE_AVATAR:
			pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pDeviceContext, LEVEL_FIRE_AVATAR));
			m_bMove = true;
			break;
		case Client::LEVEL_END:
			break;
		default:
			break;
		}

		RELEASE_INSTANCE(CGameInstance);
	}

	return S_OK;
}

CWayPoint * CWayPoint::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWayPoint* pInstance = new CWayPoint(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"CWayPoint -> Create -> pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWayPoint::Clone(void * pArg)
{
	CWayPoint* pInstance = new CWayPoint(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"CWayPoint -> Clone -> pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWayPoint::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pSphereCom);
}