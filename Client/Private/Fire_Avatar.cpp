#include "stdafx.h"
#include "..\Public\Fire_Avatar.h"
#include "GameInstance.h"
#include "Model.h"
#include "Animation.h"
#include "Channel.h"
#include "Sword.h"
#include "FireAvatar_Rock.h"
#include "Fire_Effect.h"
#include "FireAvatar_InstanceRect.h"
#include "Camera_Default.h"
#include "Lord_Balseph.h"
#include "Meteo.h"
CFire_Avatar::CFire_Avatar(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: CEnemy(pDeviceOut, pDeviceContextOut)
{

}

CFire_Avatar::CFire_Avatar(const CFire_Avatar & rhs)
	: CEnemy(rhs)
{
}

HRESULT CFire_Avatar::NativeConstruct_Prototype()
{
	if (FAILED(__super::NativeConstruct_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFire_Avatar::NativeConstruct(void * pArg)
{
	CTransform::TRANSFORMDESC      TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.SpeedPerSec = 1.0f;
	TransformDesc.RotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::NativeConstruct(pArg, &TransformDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);
	m_pCamera = ((CCamera_Default*)pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Camera"), 0));
	RELEASE_INSTANCE(CGameInstance);
	m_isBoss = true;
	m_dAnimSpeed = 0.0;
	m_pModelCom->Set_AnimationIndex(ATTACK_APPERANCE_SUB);
	m_pModelSKL->Set_AnimationIndex(ATTACK_APPERANCE_SUB);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(20.f, -28.f, 0.f, 1.f));


	if (FAILED(Ready_Effect(TEXT("FIRE_EFFECT"))))
		return E_FAIL;


	return S_OK;
}

void CFire_Avatar::Tick(_double TimeDelta)
{

	if (m_iCurrentAnimationIndex == m_iNextAnimationIndex)
	{
		m_pModelCom->Update(TimeDelta*m_dAnimSpeed, m_bCutAnimation);
		m_pModelSKL->Update(TimeDelta*m_dAnimSpeed, m_bCutAnimation);
		_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
		_vector MoveVector = XMVector3TransformCoord(XMLoadFloat3(&m_pModelCom->Get_CurAnimation()->Get_MoveVector()), m_pModelCom->Get_PivotMatrix());



		_matrix      BoneMatrix = WorldMatrix;

		BoneMatrix.r[0] = XMVector3Normalize(BoneMatrix.r[0]);
		BoneMatrix.r[1] = XMVector3Normalize(BoneMatrix.r[1]);
		BoneMatrix.r[2] = XMVector3Normalize(BoneMatrix.r[2]);
		BoneMatrix.r[3] = XMVectorSet(0.f, 0.f, 0.f, 1.f);


		MoveVector = XMVector3TransformCoord(MoveVector, BoneMatrix);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetW(m_pTransformCom->Get_State(CTransform::STATE_POSITION) + MoveVector, 1.f));

	}

	else
	{
		m_pModelCom->Update_NextAnim(TimeDelta*m_dAnimSpeed, m_iDuration, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
		m_pModelSKL->Update_NextAnim(TimeDelta*m_dAnimSpeed, m_iDuration, m_iNextAnimationIndex, &m_iCurrentAnimationIndex);
	}

	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	//CGameObject* pBalseph = pGameInstance->Get_GameObject(LEVEL_LORD_BALSEPH, TEXT("Layer_Balseph"), 0);
	//m_balsephHP = dynamic_cast<CLord_Balseph*>(pBalseph)->Get_Hp();

	//if (m_balsephHP <= 0)
	//{
	//   m_bNoHP = true;
	//}

	CTransform*      pPlayerTransform = (CTransform*)pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), TEXT("Com_Transform"));
	if (nullptr == pPlayerTransform)
		return;

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	if (m_pTransformCom->Distance(vPlayerPos, 120.f))      // ???? ????

	{
		m_bGo = true;
	}

	if (m_bGo && !m_bAppearanceEnd)
	{
		Apperance();      // ?????? ????
	}



	if (!m_bNoHP && !m_bHit)
	{
		m_TimeDelta += TimeDelta;

		if ((m_TimeDelta > 5.0) && (m_bOnce == false) && m_bAppearanceEnd)
		{
			m_bStart = true;

			//m_iMotion = 3;
			if (m_iMotion == 5)
				m_iMotion = 1;

			else
				m_iMotion++;
		}

		switch (m_iMotion)
		{
		case 0:
			SweepGround(TimeDelta);
			break;

		case 1:
			RockFalling(TimeDelta);
			break;

		case 2:
			Punch_Ground(TimeDelta);
			break;

		case 3:
			Fire_Ball(TimeDelta);
			break;

		case 4:
			TwoHandPunch_Ground(TimeDelta);
			break;

		default:
			return;
		}
	}

	m_pOBBCom->Update(m_pTransformCom->Get_WorldMatrix());
	m_pSphereCom->Update(m_pTransformCom->Get_WorldMatrix());

	__super::Tick(TimeDelta);
	RELEASE_INSTANCE(CGameInstance);
}

void CFire_Avatar::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);
	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);


	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CFire_Avatar::Render()
{
	if (nullptr == m_pShaderCom || nullptr == m_pModelCom || nullptr == m_pModelSKL)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ConstantTable()))
		return E_FAIL;


	_uint      iNumMeshContainers = m_pModelCom->Get_NumMeshContainer();

	for (_uint i = 0; i < iNumMeshContainers; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_DIFFUSE, "g_DiffuseTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_NORMALS, "g_NormalTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material_OnShader(m_pShaderCom, aiTextureType_EMISSIVE, "g_EmissiveTexture", i)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, "g_BoneMatrices", i, 4)))
			return E_FAIL;
	}

	return S_OK;
}


HRESULT CFire_Avatar::SetUp_Components()
{
	///* For.Com_Renderer */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Renderer"), LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Shader"), LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::SetUp_Components(TEXT("Com_Model"), LEVEL_FIRE_AVATAR, TEXT("Prototype_Component_Model_FireAvatar"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_ModelSKL */
	if (FAILED(__super::SetUp_Components(TEXT("Com_AvatarSKL"), LEVEL_FIRE_AVATAR, TEXT("Prototype_Component_Model_FireAvatarSKL"), (CComponent**)&m_pModelSKL)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC         ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	/* For.Com_OBB */
	ColliderDesc.vPosition = _float3(0.f, 0.f, 20.f);
	ColliderDesc.vSize = _float3(0.7f, 1.4f, 0.7f);

	if (FAILED(__super::SetUp_Components(TEXT("Com_OBB"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"), (CComponent**)&m_pOBBCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_SPHERE */
	ColliderDesc.vPosition = _float3(0.f, 0.f, 20.f);
	ColliderDesc.fRadius = 1.f;

	if (FAILED(__super::SetUp_Components(TEXT("Com_SPHERE"), LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"), (CComponent**)&m_pSphereCom, &ColliderDesc)))
		return E_FAIL;


	return S_OK;
}

HRESULT CFire_Avatar::SetUp_ConstantTable()
{
	CGameInstance*      pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pTransformCom->Bind_WorldMatrixOnShader(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_VIEW), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4_TP(CPipeLine::D3DTS_PROJ), sizeof(_float4x4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPositionFloat4(), sizeof(_float4))))
		return E_FAIL;


	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CFire_Avatar::Ready_Effect(_tchar * tagLayer)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CFire_Effect::FIREEFFECTDESC tFireEffectDesc;

#pragma region upperarm
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 36;
	tFireEffectDesc.tagBone = "upperarm_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(20.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 26;
	tFireEffectDesc.tagBone = "upperarm_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect3";
	tFireEffectDesc.vScale = _float3(20.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 0;
	tFireEffectDesc.FrameSpeed = 9;
	tFireEffectDesc.tagBone = "upperarm_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect5";
	tFireEffectDesc.vScale = _float3(20.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 36;
	tFireEffectDesc.tagBone = "upperarm_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(10.f, 5.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 31;
	tFireEffectDesc.tagBone = "upperarm_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(20.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 0;
	tFireEffectDesc.FrameSpeed = 5;
	tFireEffectDesc.tagBone = "upperarm_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect5";
	tFireEffectDesc.vScale = _float3(20.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region pinky_00
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 16;
	tFireEffectDesc.tagBone = "pinky_00_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect2";
	tFireEffectDesc.vScale = _float3(12.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 10;
	tFireEffectDesc.tagBone = "pinky_00_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect3";
	tFireEffectDesc.vScale = _float3(12.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 0;
	tFireEffectDesc.FrameSpeed = 10;
	tFireEffectDesc.tagBone = "pinky_00_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect5";
	tFireEffectDesc.vScale = _float3(12.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 17;
	tFireEffectDesc.tagBone = "pinky_00_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(12.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 10;
	tFireEffectDesc.tagBone = "pinky_00_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect2";
	tFireEffectDesc.vScale = _float3(12.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 0;
	tFireEffectDesc.FrameSpeed = 8;
	tFireEffectDesc.tagBone = "pinky_00_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect5";
	tFireEffectDesc.vScale = _float3(12.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region HLP_rollhand
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 19;
	tFireEffectDesc.tagBone = "HLP_rollhand_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect2";
	tFireEffectDesc.vScale = _float3(10.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 31;
	tFireEffectDesc.tagBone = "HLP_rollhand_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect4";
	tFireEffectDesc.vScale = _float3(10.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 10;
	tFireEffectDesc.tagBone = "HLP_rollhand_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect2";
	tFireEffectDesc.vScale = _float3(10.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 16;
	tFireEffectDesc.tagBone = "HLP_rollhand_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(10.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region KK
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 15;
	tFireEffectDesc.tagBone = "KK_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect4";
	tFireEffectDesc.vScale = _float3(8.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 11;
	tFireEffectDesc.tagBone = "KK_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(8.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 0;
	tFireEffectDesc.FrameSpeed = 12;
	tFireEffectDesc.tagBone = "KK_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect5";
	tFireEffectDesc.vScale = _float3(8.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 26;
	tFireEffectDesc.tagBone = "KK_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect3";
	tFireEffectDesc.vScale = _float3(8.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 36;
	tFireEffectDesc.tagBone = "KK_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect4";
	tFireEffectDesc.vScale = _float3(8.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 0;
	tFireEffectDesc.FrameSpeed = 15;
	tFireEffectDesc.tagBone = "KK_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect5";
	tFireEffectDesc.vScale = _float3(8.f, 10.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region clavicle
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 11;
	tFireEffectDesc.tagBone = "clavicle_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect2";
	tFireEffectDesc.vScale = _float3(10.f, 16.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.FrameSpeed = 20;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.tagBone = "clavicle_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect4";
	tFireEffectDesc.vScale = _float3(10.f, 16.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 12;
	tFireEffectDesc.tagBone = "clavicle_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(10.f, 16.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 15;
	tFireEffectDesc.tagBone = "clavicle_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect3";
	tFireEffectDesc.vScale = _float3(10.f, 16.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region thigh
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 18;
	tFireEffectDesc.tagBone = "thigh_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect3";
	tFireEffectDesc.vScale = _float3(24.f, 20.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 8;
	tFireEffectDesc.tagBone = "thigh_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(24.f, 20.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 18;
	tFireEffectDesc.tagBone = "thigh_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(24.f, 20.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 27;
	tFireEffectDesc.tagBone = "thigh_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect4";
	tFireEffectDesc.vScale = _float3(24.f, 20.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region calf
	ZeroMemory(&tFireEffectDesc, sizeof(CFire_Effect::FIREEFFECTDESC));

	tFireEffectDesc.fGlowStrength = 1.f;
	tFireEffectDesc.fOriginalStrength = 1.f;
	tFireEffectDesc.pTransformCom = m_pTransformCom;
	tFireEffectDesc.pSocketModel = m_pModelSKL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 10;
	tFireEffectDesc.tagBone = "calf_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect3";
	tFireEffectDesc.vScale = _float3(30.f, 30.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 20;
	tFireEffectDesc.tagBone = "calf_L";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(30.f, 30.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = true;
	tFireEffectDesc.bRandom = false;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 20;
	tFireEffectDesc.tagBone = "calf_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect1";
	tFireEffectDesc.vScale = _float3(30.f, 30.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;

	tFireEffectDesc.bChase = false;
	tFireEffectDesc.bRandom = true;
	tFireEffectDesc.iShaderPass = 7;
	tFireEffectDesc.FrameSpeed = 14;
	tFireEffectDesc.tagBone = "calf_R";
	tFireEffectDesc.tagTextureCom = L"Prototype_Component_Texture_FireEffect2";
	tFireEffectDesc.vScale = _float3(30.f, 30.f, 1.f);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, tagLayer, L"Prototype_GameObject_FireEffect0", &tFireEffectDesc))
		return E_FAIL;
#pragma endregion

#pragma region FireInstance
	CFireAvatar_InstanceRect::FIREINSTANCEDESC tFireInstanceDesc;
	ZeroMemory(&tFireInstanceDesc, sizeof(CFireAvatar_InstanceRect::FIREINSTANCEDESC));

	tFireInstanceDesc.pSocketModel = m_pModelSKL;
	tFireInstanceDesc.pTransformCom = m_pTransformCom;

	tFireInstanceDesc.tagBone = "KK_L";
	tFireInstanceDesc.vRevisePos = _float3(0.f, 2.f, 0.f);
	tFireInstanceDesc.pEffectDesc = pGameInstance->Get_InstanceEffect_Data(UNIT_FIREAVATAR, 2);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_Effect", L"Prototype_GameObject_FireInstance", &tFireInstanceDesc))
		return E_FAIL;

	tFireInstanceDesc.tagBone = "KK_R";
	tFireInstanceDesc.vRevisePos = _float3(0.f, 2.f, 0.f);
	tFireInstanceDesc.pEffectDesc = pGameInstance->Get_InstanceEffect_Data(UNIT_FIREAVATAR, 2);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_Effect", L"Prototype_GameObject_FireInstance", &tFireInstanceDesc))
		return E_FAIL;

	tFireInstanceDesc.tagBone = "HLP_rollhand_L";
	tFireInstanceDesc.vRevisePos = _float3(0.f, 0.f, 0.f);
	tFireInstanceDesc.pEffectDesc = pGameInstance->Get_InstanceEffect_Data(UNIT_FIREAVATAR, 2);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_Effect", L"Prototype_GameObject_FireInstance", &tFireInstanceDesc))
		return E_FAIL;

	tFireInstanceDesc.tagBone = "HLP_rollhand_R";
	tFireInstanceDesc.vRevisePos = _float3(0.f, 0.f, 0.f);
	tFireInstanceDesc.pEffectDesc = pGameInstance->Get_InstanceEffect_Data(UNIT_FIREAVATAR, 2);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_Effect", L"Prototype_GameObject_FireInstance", &tFireInstanceDesc))
		return E_FAIL;

	tFireInstanceDesc.tagBone = "HLP_thigh_L";
	tFireInstanceDesc.vRevisePos = _float3(0.f, 0.f, 0.f);
	tFireInstanceDesc.pEffectDesc = pGameInstance->Get_InstanceEffect_Data(UNIT_FIREAVATAR, 2);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_Effect", L"Prototype_GameObject_FireInstance", &tFireInstanceDesc))
		return E_FAIL;

	tFireInstanceDesc.tagBone = "HLP_thigh_R";
	tFireInstanceDesc.vRevisePos = _float3(0.f, 0.f, 0.f);
	tFireInstanceDesc.pEffectDesc = pGameInstance->Get_InstanceEffect_Data(UNIT_FIREAVATAR, 2);
	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_TUTORIAL, L"Layer_Effect", L"Prototype_GameObject_FireInstance", &tFireInstanceDesc))
		return E_FAIL;
#pragma endregion

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

void CFire_Avatar::Apperance()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce)
	{
		m_dAnimSpeed = 6.0;
		m_iDuration = 5.0;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_APPERANCE_SUB;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_APPERANCE_SUB) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 2))
	{
		m_bAppearanceEnd = true;
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}
	//224, 519

	if ((_uint)m_pModelCom->Get_Animation(ATTACK_APPERANCE_SUB)->Get_TimeAcc() == 224)
	{
		m_pCamera->Set_DownShake(true);
		//m_pCamera->Set_CommonShake(true); ?? ????
	}

	else if ((_uint)m_pModelCom->Get_Animation(ATTACK_APPERANCE_SUB)->Get_TimeAcc() == 519)
	{
		m_pCamera->Set_DownShake(true);
		//m_pCamera->Set_CommonShake(true); ?? ????
	}

	else if ((_uint)m_pModelCom->Get_Animation(ATTACK_APPERANCE_SUB)->Get_TimeAcc() == 1034)
	{
		//m_pCamera->Set_DownShake(true);
		m_pCamera->Set_CommonShake(true);
	}

	//if ((m_iCurrentAnimationIndex == ATTACK_APPERANCE_SUB) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 80
	//   && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 90)))
	//{
	//   m_bAppearanceEnd = true;
	//}
	RELEASE_INSTANCE(CGameInstance);
}

void CFire_Avatar::Fire_Ball(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = FIRE_BALL_PILLAR_START;
	}

	else if ((m_iCurrentAnimationIndex == FIRE_BALL_PILLAR_START) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = FIRE_BALL_PILLAR_LOOP;
	}

	else if ((m_iCurrentAnimationIndex == FIRE_BALL_PILLAR_LOOP) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_iNextAnimationIndex = FIRE_BALL_PILLAR_END;
	}

	else if ((m_iCurrentAnimationIndex == FIRE_BALL_PILLAR_END) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_iNextAnimationIndex = FIRE_BALL;
	}

	else if ((m_iCurrentAnimationIndex == FIRE_BALL) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_dAnimSpeed = 2.0;
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	if (((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 319)
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 330
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 350
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 370
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 390)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 4));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		//(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(-135.f));
		//Effect->Set_TimeSpeed(2.f);
	}


	if (((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 325)
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 340
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 360
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 380)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 5));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(60.f));
		//Effect->Set_TimeSpeed(2.f);
	}

	if (((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 335)
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 345
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 355
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 365
		|| (_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 375)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 5));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(5.f, 5.f, 5.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(120.f));
		//Effect->Set_TimeSpeed(2.f);
	}

	if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 390)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 9));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(60.f));
		Effect->Set_TimeSpeed(3.5f);
	}

	else if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 392)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 8));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(0.f));
		Effect->Set_TimeSpeed(3.5f);
	}

	else if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 395)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 7));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(4.f, 4.f, 4.f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(60.f));
		Effect->Set_TimeSpeed(3.5f);
	}

	else if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 400)
	{
		CMeshEffect* Effect = (CMeshEffect*)pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, TEXT("Layer_Boss_Effect"), TEXT("Prototype_GameObject_Mesh_Effect"), pGameInstance->Get_MeshEffect_Data(UNIT_BOSS, 1));
		Effect->Set_ParentsMatrix(m_pTransformCom->Get_WorldMatrix());
		(Effect->Get_Transfrom())->Scaled(_float3(2.5f, 2.5f, 2.5f));
		(Effect->Get_Transfrom())->GO_RUL(_float3(0.f, 35.f, 15.f));
		(Effect->Get_Transfrom())->Turn_Angle((Effect->Get_Transfrom())->Get_State(CTransform::STATE_LOOK), XMConvertToRadians(120.f));
		Effect->Set_TimeSpeed(10.f);
		Effect->Set_Shader(CMeshEffect::SHADER_NONEBLUR);
	}


	if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 470)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 35.f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CGameObject* pgameObject = (CGameObject*)pGameInstance->Add_GameObjectToLayer(LEVEL_FIRE_AVATAR, TEXT("Layer_Meteo"), TEXT("Prototype_GameObject_Meteo"), &vPos);
	}

	else if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_START)->Get_TimeAcc() == 490)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 35.f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CGameObject* pgameObject = (CGameObject*)pGameInstance->Add_GameObjectToLayer(LEVEL_FIRE_AVATAR, TEXT("Layer_Meteo"), TEXT("Prototype_GameObject_Meteo"), &vPos);
	}

	else if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_LOOP)->Get_TimeAcc() == 10)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 35.f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CGameObject* pgameObject = (CGameObject*)pGameInstance->Add_GameObjectToLayer(LEVEL_FIRE_AVATAR, TEXT("Layer_Meteo"), TEXT("Prototype_GameObject_Meteo"), &vPos);
	}

	else if ((_uint)m_pModelCom->Get_Animation(FIRE_BALL_PILLAR_LOOP)->Get_TimeAcc() == 25)
	{
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0.f, 35.f, 0.f, 0.f) + m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		CGameObject* pgameObject = (CGameObject*)pGameInstance->Add_GameObjectToLayer(LEVEL_FIRE_AVATAR, TEXT("Layer_Meteo"), TEXT("Prototype_GameObject_Meteo"), &vPos);
	}

	RELEASE_INSTANCE(CGameInstance);

}

void CFire_Avatar::SweepGround(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = CLEAVE_RIGHT;
	}

	else if ((m_iCurrentAnimationIndex == CLEAVE_RIGHT) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFire_Avatar::RockFalling(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_bCrashRock = false;
		m_bCreateRock = false;
		m_dAnimSpeed = 2.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = ATTACK_ROCK_THROW;
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_ROCK_THROW) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	if ((m_iCurrentAnimationIndex == ATTACK_ROCK_THROW) && !m_bCreateRock)
	{
		if (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 220 && m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 227)
		{
			CSword::SOCKETDESC         SocketDesc;
			ZeroMemory(&SocketDesc, sizeof(CSword::SOCKETDESC));

			SocketDesc.pModelCom = (CModel*)pGameInstance->Get_Component(LEVEL_FIRE_AVATAR, TEXT("Layer_FireAvatar"), TEXT("Com_AvatarSKL"));
			SocketDesc.pBoneName = "KK_R";

			if (nullptr == (pGameInstance->Add_GameObjectToLayer(LEVEL_FIRE_AVATAR, TEXT("Layer_FireAvatar_Rock"), TEXT("Prototype_GameObject_FireAvatar_Rock"), &SocketDesc)))
				return;

			m_bCreateRock = true;
			m_bCreateSmallRock = false;
		}
	}

	else if ((m_iCurrentAnimationIndex == ATTACK_ROCK_THROW) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= 594) && m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() <= 605)
	{
		if (!m_bCrashRock)
		{
			CGameObject* pRock = pGameInstance->Get_GameObject(LEVEL_FIRE_AVATAR, TEXT("Layer_FireAvatar_Rock"), 0);
			m_vStorePos = dynamic_cast<CFireAvatar_Rock*>(pRock)->Get_Position();
		}
		m_bCrashRock = true;
		if (!m_bCreateSmallRock)
		{
			float j = XMVectorGetX(m_vStorePos) + (rand() % 15 / 100.f);
			float i = XMVectorGetZ(m_vStorePos) + (rand() % 10 / 100.f);
			for (int k = 0; k < 5; ++k)
			{
				m_vStorePos = XMVectorSet(i, XMVectorGetY(m_vStorePos), j, 0.f);
				if (nullptr == (pGameInstance->Add_GameObjectToLayer(LEVEL_LORD_BALSEPH, TEXT("Layer_Falling_Rock"), TEXT("Prototype_GameObject_Falling_Rock"), &m_vStorePos)))
					return;
			}
			m_bCreateSmallRock = true;
		}
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFire_Avatar::Punch_Ground(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = CRUSH_CENTER;
	}

	else if ((m_iCurrentAnimationIndex == CRUSH_CENTER) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	if ((m_iCurrentAnimationIndex == CRUSH_CENTER) && ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 420))
		|| ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 421))
		|| ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 422))
		|| ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 423)))
	{
		m_pCamera->Set_DownShake(true);
		//m_pCamera->Set_CommonShake(true); ?? ????
	}
	RELEASE_INSTANCE(CGameInstance);
}

void CFire_Avatar::TwoHandPunch_Ground(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = CHRUSH_BOTH_ARMS;
	}

	else if ((m_iCurrentAnimationIndex == CHRUSH_BOTH_ARMS) && (m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() >= m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_NumeKeyFrames() - 1))
	{
		m_bCutAnimation = true;
		m_iNextAnimationIndex = MOVE_IDLE;
		m_bOnce = false;
		m_TimeDelta = 0.0;
	}

	if ((m_iCurrentAnimationIndex == CHRUSH_BOTH_ARMS) && ((m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 432
		|| m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 433
		|| m_pModelCom->Get_CurAnimation()->Get_PelvisChannel()->Get_CurrentKeyFrameIndex() == 434)))
	{
		m_pCamera->Set_DownShake(true);
		//m_pCamera->Set_CommonShake(true); ?? ????
	}

	RELEASE_INSTANCE(CGameInstance);
}

void CFire_Avatar::NoMoreLife(_double TimeDelta)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (!m_bOnce && m_bStart)
	{
		m_dAnimSpeed = 2.0;
		m_iDuration = 5.0;
		m_bStart = false;
		m_bOnce = true;
		m_bCutAnimation = false;
		m_iNextAnimationIndex = DEAD;
		m_bOnce = false;
	}

	RELEASE_INSTANCE(CGameInstance);
}


CFire_Avatar * CFire_Avatar::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
{
	CFire_Avatar*   pInstance = new CFire_Avatar(pDeviceOut, pDeviceContextOut);

	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created CFire_Avatar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFire_Avatar::Clone(void * pArg)
{
	CFire_Avatar*   pInstance = new CFire_Avatar(*this);

	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(TEXT("Failed to Created Clone CFire_Avatar"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFire_Avatar::Free()
{
	Safe_Release(m_pModelSKL);
	Safe_Release(m_pModelCom);
	__super::Free();
}