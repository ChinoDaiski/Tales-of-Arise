#include "stdafx.h"
#include "..\Public\WayPoint_Pos.h"
#include "Player.h"

#include "Level_Loading.h"
#include "Navigation.h"
#include "Player.h"

CWayPoint_Pos::CWayPoint_Pos(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CWayPoint_Pos::CWayPoint_Pos(const CWayPoint_Pos & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWayPoint_Pos::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
	{
		MSG_BOX(L"CWayPoint_Pos -> NativeConstruct_Prototype -> NativeConstruct_Prototype");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWayPoint_Pos::NativeConstruct(void * pArg, CTransform::TRANSFORMDESC* pTransformDesc)
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

void CWayPoint_Pos::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

#ifdef _DEBUG
	// 충돌체 업데이트
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
#endif // _DEBUG

	Check_TargetPlayer_Collision();
}

void CWayPoint_Pos::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

#ifdef _DEBUG
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);

	RELEASE_INSTANCE(CGameInstance);
#endif // _DEBUG
}

HRESULT CWayPoint_Pos::Render()
{
#ifdef _DEBUG
	m_pSphereCom->Render();
#endif // _DEBUG

	return S_OK;
}

HRESULT CWayPoint_Pos::SetUp_Components(void * pArg)
{
	if (nullptr == pArg)
		return E_FAIL;

	WayPoint_DESC Desc = *((WayPoint_DESC*)pArg);

	// 위치 설정
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(Desc.fPos.x, Desc.fPos.y, Desc.fPos.z, 1.f));

	m_fMovePos = Desc.fMovePos;

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

HRESULT CWayPoint_Pos::SetUp_ConstantTable()
{

	return S_OK;
}

HRESULT CWayPoint_Pos::Check_TargetPlayer_Collision(void)
{
	if (m_bMove)
		return S_OK;

	if (nullptr == m_pTargetPlayer) {
		MSG_BOX(L"CWayPoint_Pos : No Player Detected!");
		return E_FAIL;
	}

	CCollider* pPlayer_Collider = dynamic_cast<CCollider*>(m_pTargetPlayer->Get_Component(TEXT("Com_SPHERE_Interaction")));

	if (nullptr == pPlayer_Collider) {
		MSG_BOX(L"CWayPoint_Pos : No Collider Detected!");
		return E_FAIL;
	}

	// 자신의 충돌체와 플레이어의 상호작용 충돌체가 충돌했을 경우
	if (m_pSphereCom->Collision(pPlayer_Collider)) {
		dynamic_cast<CTransform*>(m_pTargetPlayer->Get_Component(TEXT("Com_Transform")))->Move(m_fMovePos);

		CNavigation* pNaviCom = m_pTargetPlayer->Get_NaviCom();

		pNaviCom->Find_My_Cell(XMVectorSet(m_fMovePos.x, m_fMovePos.y, m_fMovePos.z, 1.f));
		m_bMove = true;
	}

	return S_OK;
}

CWayPoint_Pos * CWayPoint_Pos::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CWayPoint_Pos* pInstance = new CWayPoint_Pos(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"CWayPoint_Pos -> Create -> pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWayPoint_Pos::Clone(void * pArg)
{
	CWayPoint_Pos* pInstance = new CWayPoint_Pos(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"CWayPoint_Pos -> Clone -> pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWayPoint_Pos::Free()
{
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pSphereCom);
}