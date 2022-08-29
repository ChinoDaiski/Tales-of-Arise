#include "stdafx.h"
#include "..\Public\Meteo.h"
#include "GameInstance.h"
//#include "MeshEffect.h"
CMeteo::CMeteo(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CMeteo::CMeteo(const CMeteo & rhs)
	: CGameObject(rhs)
{
}

HRESULT CMeteo::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeteo::NativeConstruct(void* pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 15.f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		_vector pPos = *(_vector*)pArg;
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, pPos);
	}

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CMeteo::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	if (!m_bOnlyOnce)
	{
		CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
		if (nullptr == pPlayerTransform)
			return;

		vPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		EffectMesh = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Meteo_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 14));
		//EffectMesh->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		//(EffectMesh->Get_Transfrom())->Scaled(_float3(0.5f, 0.5f, 0.5f));
		//(EffectMesh->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -3.f));
		////EffectMesh->Set_TimeSpeed(1.f);
		//vPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_bOnlyOnce = true;
	}

	_float fPos = XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (fPos < 0.5f)
	{
		//if(EffectMesh!= nullptr)
		//	EffectMesh->Set_Dead(true);

		m_bDead = true;
	}


	// 여기가 터진다
	// EffectMesh->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());


	(EffectMesh->Get_Transfrom())->Scaled(_float3(0.5f, 0.5f, 0.5f));
	(EffectMesh->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -3.f));
	EffectMesh->Set_TimeSpeed(8.f);
	EffectMesh->Set_Shader(CMeshEffect::SHADER_DEFAULT, _float2(3.f, 3.f));

	m_pTransformCom->LookAt(vPos);
	m_pTransformCom->Go_Straight(TimeDelta);




	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());


	RELEASE_INSTANCE(CGameInstance);
}

void CMeteo::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
}

HRESULT CMeteo::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;



#ifdef _DEBUG
	m_pSphereCom->Render();
#endif // _DEBUG


	return S_OK;
}

HRESULT CMeteo::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMeteo::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);




	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CMeteo * CMeteo::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CMeteo*	pInstance = new CMeteo(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CMeteo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CMeteo::Clone(void * pArg)
{
	CMeteo*	pInstance = new CMeteo(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone CMeteo"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMeteo::Free()
{
	__super::Free();

	Safe_Release(m_pSphereCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}