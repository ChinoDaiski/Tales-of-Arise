#include "stdafx.h"
#include "Dot.h"
#include "ImGui_Manager.h"

CDot::CDot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CGameObject(pDevice, pDeviceContext)
{
}

CDot::CDot(const CDot & rhs)
	:CGameObject(rhs)
{
}

HRESULT CDot::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDot::NativeConstruct(void* pArg)
{
	if (FAILED(__super::NativeConstruct(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components(pArg)))
		return E_FAIL;

	DotDesc* Desc = (DotDesc*)pArg;

	_vector vPos = XMLoadFloat3(&Desc->fPos);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(vPos, 1.f));

	return S_OK;
}

void CDot::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CDot::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (CImGUI_Manager::GetInstance()->Get_Level() == CImGUI_Manager::UL_Navigation) {
		if (nullptr != m_pRendererCom) {
			;
#ifdef _DEBUG
			if (true == pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State(CTransform::STATE_POSITION), 3.f))
				m_pRendererCom->Add_DebugComponent(m_pSphereCom);
#endif // _DEBUG
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CDot::Render(void)
{

	return S_OK;
}

HRESULT CDot::SetUp_Components(void * pArg)
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_AABB */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vSize = _float3(0.6f, 0.6f, 0.6f);	// _float3(1.f, 1.f, 1.f);
	ColliderDesc.fRadius = ColliderDesc.vSize.y * 0.5f;
	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_Sphere"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

CGameObject * CDot::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CDot* pInstance = new CDot(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CDot::Clone(void * pArg)
{
	CDot* pInstance = new CDot(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"CSelect_Player -> Create -> pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CDot::Free()
{
	__super::Free();
	
	Safe_Release(m_pSphereCom);
}
