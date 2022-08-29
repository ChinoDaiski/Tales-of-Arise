#include "stdafx.h"
#include "..\Public\Gravitas_Field.h"
#include "GameInstance.h"
#include "Animation.h"
#include "MeshEffect.h"
#include "Rect_Effect.h"
#include"Channel.h"
#include "Shionne.h"
CGravitas_Field::CGravitas_Field(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CBullet(pDeviceOut, pDeviceContextOut)
{

}

CGravitas_Field::CGravitas_Field(const CGravitas_Field & rhs)
	: CBullet(rhs)
{
}

HRESULT CGravitas_Field::NativeConstruct_Prototype()
{

	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGravitas_Field::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 5.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);


	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;



	if (pArg != nullptr)
	{
		memcpy(&m_BulletDesc, pArg, sizeof(BULLETDESC));
		m_pSocketMatrix = m_BulletDesc.pModelCom->Get_CombinedTransformationMatrix(m_BulletDesc.pBoneName);

		m_PivotMatrix = m_BulletDesc.pModelCom->Get_PivotMatrix4x4();
	}

	if (nullptr == m_pSocketMatrix)
		return E_FAIL;
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	/* 뼈의 스케일상태는 무시. 회전값과 이동값만 무기의 상태로 사용한다. */
	_matrix		BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), CGameObject::m_pTransformTag, 1);
	if (nullptr == pPlayerTransform)
		return E_FAIL;

	_float4x4	SocketMatrix;
	XMStoreFloat4x4(&SocketMatrix, (BoneMatrix * XMLoadFloat4x4(&m_PivotMatrix) * pPlayerTransform->Get_WorldMatrix()));



	m_pTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix()*XMLoadFloat4x4(&SocketMatrix));


	//m_pTransformCom->TurnAxis(XMVectorSetY(m_BulletDesc.vTargetPos, 0.f) - XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION),0.f));


	m_pTransformCom->Scaled(_float3(1.f, 1.f, 1.f));
	m_pAttackSphereCom->Set_On(true);
	RELEASE_INSTANCE(CGameInstance);


	m_DeadTime = 2.0;
	m_LiveTime = 0.0;


	m_pParticle1 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 3));
	//m_pParticle1->Set_Transform(UNIT_SHIONNE, this,m_pTransformCom,_float3(0.f,0.f,0.f));
	//m_pParticle1->Set_ParentsMatrix(XMLoadFloat4x4(&SocketMatrix));
	m_pParticle1->Set_Blur(_float2(3.f, 3.f));
	m_pParticle1->Get_CreateDesc()->vColor1 = _float4(1.f, 0.5f, 1.f, 1.f);
	m_pParticle3 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 3));
	m_pParticle3->Set_Blur(_float2(3.f, 3.f));
	m_pParticle3->Get_CreateDesc()->vColor1 = _float4(1.f, 0.5f, 1.f, 1.f);
	m_pParticle2 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 4));
	m_pParticle2->Set_Blur(_float2(3.f, 3.f));
	m_pParticle2->Get_CreateDesc()->vColor1 = _float4(1.f, 0.5f, 1.f, 1.f);
	//m_pParticle2->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));
	//m_pParticle2->Set_ParentsMatrix(XMLoadFloat4x4(&SocketMatrix));




	return S_OK;
}

void CGravitas_Field::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);


	//m_dAliveTime += TimeDelta;


	//if (m_dAliveTime > 4.0)
	//{
	//	m_bDead = true;
	//	return;
	//}


	//m_pSocketMatrix = m_BulletDesc.pModelCom->Get_CombinedTransformationMatrix(m_BulletDesc.pBoneName);
	//m_PivotMatrix = m_BulletDesc.pModelCom->Get_PivotMatrix4x4();

	//CTransform*		pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), CGameObject::m_pTransformTag, 1);


	//_matrix		BoneMatrix = XMLoadFloat4x4(m_pSocketMatrix);

	//BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
	//BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
	//BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);

	//_float4x4	SocketMatrix;
	//XMStoreFloat4x4(&SocketMatrix, (BoneMatrix * XMLoadFloat4x4(&m_PivotMatrix)* pPlayerTransform->Get_WorldMatrix()));

	//*(_float4*)&SocketMatrix.m[3][0];
	//_vector vParticlePos = XMLoadFloat4((_float4*)&SocketMatrix.m[3][0]) - XMVector3Normalize(XMLoadFloat4((_float4*)&SocketMatrix.m[2][0]))*0.8f;


	//if (m_pParticle1 != nullptr)
	//{
	//	if (!m_pParticle1->Get_Finish())
	//	{

	//		m_pParticle1->Set_Pos(vParticlePos);

	//	}

	//	else {
	//		m_pParticle1 = nullptr;
	//	}
	//}


	//if (m_pParticle2 != nullptr)
	//{
	//	if (!m_pParticle2->Get_Finish())
	//		m_pParticle2->Set_Pos(vParticlePos);
	//	else
	//		m_pParticle2 = nullptr;
	//}

	//if (m_pParticle3 != nullptr)
	//{
	//	if (!m_pParticle3->Get_Finish())
	//		m_pParticle3->Set_Pos(vParticlePos);
	//	else
	//		m_pParticle3 = nullptr;
	//}


	///*
	//m_pParticle4 = nullptr; //발사
	//m_pParticle5 = nullptr; //날라갈떄
	//m_pParticle6 = nullptr; //터질때
	//m_pParticle7 = nullptr;  //구체내부

	//*/

	////57틱에 구체생성  90틱에 발사,이펙트 날아감 
	//if (m_BulletDesc.pModelCom->Get_CurAnimationIndex() == CShionne::SIO_ANIM_STATE::SIO_ANIM_BTL_ATTACK_GRAVITY_FORCE)
	//{

	//	if (m_BulletDesc.pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() > 57 && m_BulletDesc.pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <90) //구체생성 
	//	{
	//		if (!m_bOnce)
	//		{
	//			if (!m_bMakeBullet)
	//			{
	//				m_pSmallSphere = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 12));
	//				m_pSmallSphere->Set_ParentsMatrix(XMLoadFloat4x4(&SocketMatrix));

	//				(m_pSmallSphere->Get_Transfrom())->Scaled(_float3(1.5f, 1.5f, 1.5f));
	//				(m_pSmallSphere->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -1.f));
	//				//EffectMesh->Get_Transfrom()->Turn_Angle(Get_PlayerLook(), XMConvertToRadians(-5.f));
	//				m_pSmallSphere->Set_TimeSpeed(1.f);
	//				m_pSmallSphere->Set_Finish_Dead(false);


	//				m_pBigSphere = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 21));
	//				m_pBigSphere->Set_ParentsMatrix(XMLoadFloat4x4(&SocketMatrix));
	//				(m_pBigSphere->Get_Transfrom())->Scaled(_float3(1.5f, 1.5f, 1.5f));
	//				(m_pBigSphere->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -1.f));
	//				//EffectMesh->Get_Transfrom()->Turn_Angle(Get_PlayerLook(), XMConvertToRadians(-5.f));
	//				m_pBigSphere->Set_TimeSpeed(1.f);
	//				m_pBigSphere->Set_Finish_Dead(false);


	//				m_pBigSphere->Get_Transfrom()->LookAt(pGameInstance->Get_CamPosition());


	//				m_pParticle1->Set_TimeEffect(m_pParticle1->Get_CreateDesc()->fMaxTime);
	//				m_pParticle2->Set_TimeEffect(m_pParticle2->Get_CreateDesc()->fMaxTime);
	//				m_pParticle3->Set_TimeEffect(m_pParticle3->Get_CreateDesc()->fMaxTime);


	//				m_bMakeBullet = true;

	//			}
	//			else {


	//				m_pSmallSphere->Set_ParentsMatrix(XMLoadFloat4x4(&SocketMatrix));

	//				(m_pSmallSphere->Get_Transfrom())->Scaled(_float3(1.5f, 1.5f, 1.5f));
	//				(m_pSmallSphere->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -1.f));


	//				m_pBigSphere->Set_ParentsMatrix(XMLoadFloat4x4(&SocketMatrix));
	//				(m_pBigSphere->Get_Transfrom())->Scaled(_float3(1.5f, 1.5f, 1.5f));
	//				(m_pBigSphere->Get_Transfrom())->GO_RUL(_float3(0.f, 0.f, -1.f));
	//			}

	//		}



	//	}

	//	if (m_BulletDesc.pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() > 90)
	//	{
	//		m_bOnce = true;
	//		if (!m_bFireBullet)
	//		{
	//			m_pSmallSphere->Set_Dead(true);
	//			m_pBigSphere->Set_Dead(true);
	//			m_pSmallSphere2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 20));
	//			m_pBigSphere2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 21));

	//			m_pSmallSphere2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
	//			m_pBigSphere2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());

	//			m_pSmallSphere2->Set_Finish_Dead(false);
	//			m_pBigSphere2->Set_Finish_Dead(false);

	//			m_pParticle4 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 5));
	//			m_pParticle4->Set_Pos(vParticlePos);
	//			m_pParticle4->Set_TimeSpeed(0.1f);

	//			m_pParticle5 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 6));
	//			m_pParticle5->Set_Pos(vParticlePos);
	//			m_pParticle5->Set_TimeSpeed(0.1f);

	//			m_pParticle6 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 7));
	//			m_pParticle7 = (CRect_Effect*)pGameInstance->Add_GameObjectToLayer(pGameInstance->Get_LevelIndex(), TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Rect_Effect"), pGameInstance->Get_InstanceEffect_Data(UNIT_SHIONNE, 8));

	//			m_pParticle6->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));
	//			m_pParticle7->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));

	//			m_pParticle6->Set_TimeSpeed(1.f);
	//			m_pParticle7->Set_TimeSpeed(1.f);



	//			//m_pWave1 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 13));
	//			//m_pWave2 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 14));
	//			//m_pWave3 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 15));
	//			//m_pWave4 = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Player_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_SHIONNE, 16));
	//			//m_pWave1->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));
	//			//m_pWave2->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));
	//			//m_pWave3->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));
	//			//m_pWave4->Set_Transform(UNIT_SHIONNE, this, m_pTransformCom, _float3(0.f, 0.f, 0.f));


	//			m_bFireBullet = true;

	//		}



	//	}


	//}


	//if (m_bFireBullet)
	//{
	//	m_pSmallSphere2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
	//	m_pBigSphere2->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
	//	m_pBigSphere2->Get_Transfrom()->LookAt(pGameInstance->Get_CamPosition());
	//	m_pTransformCom->Go_Straight_PlaneXZ(-TimeDelta);
	//}













	RELEASE_INSTANCE(CGameInstance);

	m_pAttackSphereCom->Update(m_pTransformCom->Get_WorldMatrix());
}

void CGravitas_Field::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);



#ifdef DEBUG

	m_pRendererCom->Add_DebugComponent(m_pAttackSphereCom);
#endif // DEBUG

}

HRESULT CGravitas_Field::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	//if (FAILED(SetUp_ConstantTable()))
	//	return E_FAIL;

	////16
	//for (_uint i = 0; i < iNumMeshContainers; ++i)
	//{
	//	if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
	//		return E_FAIL;


	//	if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 3)))
	//		return E_FAIL;
	//}


#ifdef _DEBUG

	//m_pOBBCom->Render();
	m_pAttackSphereCom->Render();
#endif // _DEBUG
	return S_OK;
}

HRESULT CGravitas_Field::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	///* For.Com_Model */
	//if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_STATIC, TEXT("Prototype_Component_Model_Spear"), (CComponent**)&m_pModelCom)))
	//	return E_FAIL;
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));


	ColliderDesc.vPosition = _float3(0.f, 0.f, 0.5f);
	ColliderDesc.vSize = _float3(0.15f, 0.3f, 1.2f);
	ColliderDesc.fRadius = 0.1f;
	if (FAILED(__super::SetUp_Components(TEXT("Com_AttackSPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pAttackSphereCom, &ColliderDesc)))
		return E_FAIL;

	//m_pAttackSphereCom->Set_HitInfo(1, false);

	return S_OK;
}

HRESULT CGravitas_Field::SetUp_ConstantTable()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;



	_float4x4	SocketMatrix;
	XMStoreFloat4x4(&SocketMatrix, XMMatrixIdentity());
	m_pShaderCom->Set_RawValue("g_SocketMatrix", &SocketMatrix, sizeof(_float4x4));





	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CGravitas_Field * CGravitas_Field::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CGravitas_Field*	pInstance = new CGravitas_Field(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CGravitas_Field"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CGravitas_Field::Clone(void * pArg)
{
	CGravitas_Field*	pInstance = new CGravitas_Field(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created CGravitas_Field"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CGravitas_Field::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);

	Safe_Release(m_pRendererCom);


	if (m_pBigSphere != nullptr)
		m_pBigSphere->Set_Dead(true);

	if (m_pBigSphere2 != nullptr)
		m_pBigSphere2->Set_Dead(true);

	if (m_pSmallSphere != nullptr)
		m_pSmallSphere->Set_Dead(true);

	if (m_pSmallSphere2 != nullptr)
		m_pSmallSphere2->Set_Dead(true);

	if (m_pParticle1 != nullptr)
		m_pParticle1->Set_TimeEffect(m_pParticle1->Get_CreateDesc()->fMaxTime);
	if (m_pParticle2 != nullptr)
		m_pParticle2->Set_TimeEffect(m_pParticle2->Get_CreateDesc()->fMaxTime);
	if (m_pParticle3 != nullptr)
		m_pParticle3->Set_TimeEffect(m_pParticle3->Get_CreateDesc()->fMaxTime);
	if (m_pParticle4 != nullptr)
		m_pParticle4->Set_TimeEffect(m_pParticle4->Get_CreateDesc()->fMaxTime);
	if (m_pParticle5 != nullptr)
		m_pParticle5->Set_TimeEffect(m_pParticle5->Get_CreateDesc()->fMaxTime);

	if(m_pParticle6 != nullptr)
		m_pParticle6->Set_Dead(true);
	if (m_pParticle7 != nullptr)
		m_pParticle7->Set_Dead(true);
	/*
	if (m_pParticle6 != nullptr)
	m_pParticle6->Set_TimeEffect(m_pParticle6->Get_CreateDesc()->fMaxTime);
	if (m_pParticle7 != nullptr)
	m_pParticle7->Set_TimeEffect(m_pParticle7->Get_CreateDesc()->fMaxTime);
	*/

}
