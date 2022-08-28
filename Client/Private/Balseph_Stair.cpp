#include "stdafx.h"
#include "..\Public\Balseph_Stair.h"

#include "GameInstance.h"

#include "Collider.h"
#include "Animation.h"

#include "Player.h"
#include "UI_Owl.h"	
#include "Lord_Balseph.h"
#include "WayPoint.h"

CBalseph_Stair::CBalseph_Stair(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CGameObject(pDeviceOut, pDeviceContextOut)
{

}

CBalseph_Stair::CBalseph_Stair(const CBalseph_Stair & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBalseph_Stair::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : NativeConstruct_Prototype : NativeConstruct_Prototype", E_FAIL);

	return S_OK;
}

HRESULT CBalseph_Stair::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : NativeConstruct : NativeConstruct", E_FAIL);

	if (FAILED(SetUp_Components(pArg)))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : NativeConstruct : SetUp_Components", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	// 계단은 애니메이션이 내려오는것 1개밖에 없다.
	m_pModelCom->Set_AnimationIndex(0);

	return S_OK;
}

void CBalseph_Stair::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bActivate) {
		if (nullptr != m_pTargetEnemy) {

			_int iMaxHP = m_pTargetEnemy->Get_MaxHp();
			_int iHP = m_pTargetEnemy->Get_Hp();

			if (iMaxHP * 0.5 >= iHP)
				m_bActivate = true;
		}
	}
	else {
		m_pModelCom->Update(TimeDelta, false);

		// WayPoint를 만들지 않았다면
		static _bool bMakeWayPoint = false;

		// 해당 애니메이션이 300프레임을 넘어간다면
		if (m_pModelCom->Get_Animation(0)->Get_TimeAcc() > 300) {
			if (!bMakeWayPoint) {
				// 계단 아래쪽에 움직이는 맵을 이동하는 WayPoint를 설치한다.
				// 해당 WayPoint는 플레이어와 충돌시 FireAvatar로 이동하는 오브젝트이다.

				CWayPoint::WayPoint_DESC WayDesc;
				WayDesc.eLevel = LEVEL_FIRE_AVATAR;				// 이동할다음 레벨
				WayDesc.fPos = _float3(-86.12, 12.55, 0.03);	// 오브젝트 생성위치
				WayDesc.pTargetPlayer = m_pTargetPlayer;		// 타겟이 되는 플레이어 오브젝트 정보

				CWayPoint* pWayPoint = dynamic_cast<CWayPoint*>(pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, LAYER_WayPoints, TEXT("Prototype_GameObject_WayPoint"), &WayDesc));

				if (nullptr == pWayPoint) {
					return;
				}

				pWayPoint->Set_TargetPlayer(m_pTargetPlayer);

				bMakeWayPoint = true;
			}
		}
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

void CBalseph_Stair::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (nullptr != m_pRendererCom){
		m_pRendererCom->Add_RenderGroup(CRenderer::GROUP_NONBLEND, this);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CBalseph_Stair::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pModelCom)
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : nullptr", E_FAIL);

	if (FAILED(__super::Render()))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : Render", E_FAIL);

	if (FAILED(SetUp_ConstantTable()))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : SetUp_ConstantTable", E_FAIL);

	_uint      iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : Bind_Material_OnShader(g_DiffuseTexture)", E_FAIL);
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i)))
			MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : Bind_Material_OnShader(g_NormalTexture)", E_FAIL);
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_EMISSIVE, "g_EmissiveTexture", i)))
			MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : Bind_Material_OnShader(g_EmissiveTexture)", E_FAIL);

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 0)))
			MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : Render : Render", E_FAIL);
	}
}

HRESULT CBalseph_Stair::SetUp_Components(void* pArg)
{
	if (nullptr == pArg)
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_Components : nullptr == pArg", E_FAIL);

	Balseph_StairDesc Desc = *((Balseph_StairDesc*)pArg);

	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_Components : SetUp_Components(Com_Renderer)", E_FAIL);

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_Components : SetUp_Components(Com_Shader)", E_FAIL);

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_STATIC, Desc.pModelTag, (CComponent**)&m_pModelCom)))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_Components : SetUp_Components(Com_Model)", E_FAIL);

	m_pTargetEnemy = Desc.pTargetEnemy; 
	m_pTargetPlayer = Desc.pTargetPlayer;

	return S_OK;
}

HRESULT CBalseph_Stair::SetUp_ConstantTable()
{
	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_ConstantTable : Bind_WorldMatrixOnShader(g_WorldMatrix)", E_FAIL);

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_ConstantTable : Set_RawValue(g_ViewMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_ConstantTable : Set_RawValue(g_ProjMatrix)", E_FAIL);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4))))
		MSG_CHECK_RETURN(L"Failed To CBalseph_Stair : SetUp_ConstantTable : Set_RawValue(g_vCamPosition)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CBalseph_Stair * CBalseph_Stair::Create(ID3D11Device * pDeviceOut, ID3D11DeviceContext * pDeviceContextOut)
{
	CBalseph_Stair*   pInstance = new CBalseph_Stair(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created Owl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBalseph_Stair::Clone(void * pArg)
{
	CBalseph_Stair*   pInstance = new CBalseph_Stair(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created Clone Owl"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBalseph_Stair::Free()
{
	__super::Free();

	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pModelCom);
}