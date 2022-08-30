#include "stdafx.h"
#include "..\Public\Loader_etc.h"

#include "GameInstance.h"
#include "Camera_Default.h"
#include "LightDepth.h"

#include "Navigation.h"
#include "Sky.h"

#include "Fire_Deco.h"
#include "Fire_Effect.h"

#include "Dissolve.h"

CLoader_etc::CLoader_etc(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut)
	: m_pDevice(pDeviceOut), m_pDeviceContext(pDeviceContextOut)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CRITICAL_SECTION CLoader_etc::Get_CriticalSection()
{
	return m_CriticalSection;
}

LEVEL CLoader_etc::Get_LevelIndex()
{
	return m_eLevel;
}

_bool CLoader_etc::Get_Finished() const
{
	return m_isFinished;
}

const _tchar* CLoader_etc::Get_LoadingText() const
{
	return m_szLoading;
}

unsigned int APIENTRY ThreadEntryFunc(void* pArg)
{
	CLoader_etc*		pLoader = (CLoader_etc*)pArg;

	EnterCriticalSection(&pLoader->Get_CriticalSection());

	switch (pLoader->Get_LevelIndex())
	{
	case LEVEL_LOBBY:
		pLoader->Loading_ForLobbyLevel();
		break;

	case LEVEL_TUTORIAL:
		pLoader->Loading_ForTutorialLevel();
		break;

	case LEVEL_LORD_BALSEPH:
		pLoader->Loading_ForBoss1Level();
		break;

	case LEVEL_FIRE_AVATAR:
		pLoader->Loading_ForBoss2Level();
		break;

	case LEVEL_TEST:
		pLoader->Loading_ForTestLevel();
		break;
	}

	LeaveCriticalSection(&pLoader->Get_CriticalSection());

	return 0;
}

HRESULT CLoader_etc::NativeConstruct(LEVEL eLevel)
{
	m_eLevel = eLevel;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc, this, 0, nullptr);
	if (0 == m_hThread)
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : NativeConstruct : m_hThread", E_FAIL);

	return S_OK;
}

HRESULT CLoader_etc::Loading_ForLobbyLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Lobby_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForLobbyLevel : Loading_Lobby_Model", E_FAIL);

	if (FAILED(Loading_Lobby_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForLobbyLevel : Loading_Lobby_Texture", E_FAIL);

	if (FAILED(Loading_Lobby_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForLobbyLevel : Loading_Lobby_Object", E_FAIL);

	if (FAILED(Loading_Lobby_Components()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForLobbyLevel : Loading_Lobby_Components", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_etc::Loading_Lobby_Model()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Lobby_Texture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//화면 전환
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Texture_Dissolve_Sour");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve_Sour"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Dissolve/Dissolve_Sour%d.dds"), 5))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_Dissolve_Sour)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Texture_Dissolve_Dest");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Dissolve_Dest"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Dissolve/Dissolve_Dest%d.dds"), 1))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_Dissolve_Dest)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_Lobby_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_Dissolve");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Dissolve"),
		CDissolve::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Lobby_Object : Add_Prototype(Prototype_GameObject_Dissolve)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_Lobby_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region Collider
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Collider_AABB");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_AABB))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Lobby_Components : Add_Prototype(Prototype_Component_Collider_AABB)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Collider_OBB");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_OBB))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Lobby_Components : Add_Prototype(Prototype_Component_Collider_OBB)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Collider_SPHERE");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Collider_SPHERE"),
		CCollider::Create(m_pDevice, m_pDeviceContext, CCollider::TYPE_SPHERE))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Lobby_Components : Add_Prototype(Prototype_Component_Collider_SPHERE)", E_FAIL);
#pragma endregion

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_ForTutorialLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Tutorial_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForTutorialLevel : Loading_Tutorial_Model", E_FAIL);

	if (FAILED(Loading_Tutorial_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForTutorialLevel : Loading_Tutorial_Texture", E_FAIL);

	if (FAILED(Loading_Tutorial_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForTutorialLevel : Loading_Tutorial_Object", E_FAIL);

	if (FAILED(Loading_Tutorial_Components()))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_ForTutorialLevel : Loading_Tutorial_Components", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_etc::Loading_Tutorial_Model()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Tutorial_Texture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Texture_SkyBox");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_SkyBox"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/SkyBox/Skybox%d.dds"), 15))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_SkyBox)", E_FAIL);

	// Owl UI용 텍스쳐
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Texture_InteractionUI");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_InteractionUI"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/UMap/MapObejct/Owl_UI/interactionUI.dds"), 1))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_InteractionUI)", E_FAIL);
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Texture_message_box_context");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_message_box_context"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/UMap/MapObejct/Owl_UI/message_box_context.dds"), 1))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_message_box_context)", E_FAIL);
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Texture_message_box_name");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_message_box_name"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/UMap/MapObejct/Owl_UI/message_box_name.dds"), 1))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_message_box_name)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader_etc::Loading_Tutorial_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_Camera_Default");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Default"),
		CCamera_Default::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Camera_Default)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_LightDepth");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_LightDepth"),
		CLightDepth::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_LightDepth)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_Sky");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Sky"),
		CSky::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Sky)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_Fire");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Fire"),
		CFire_Deco::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Fire)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_FireEffect0");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireEffect0"),
		CFire_Effect::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_etc : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_FireEffect0)", E_FAIL);

	Safe_Release(pGameInstance);
	return S_OK;
}

HRESULT CLoader_etc::Loading_Tutorial_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

#pragma region Shader
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_VtxNorTex");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_VtxAnim");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnim.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_VtxInstance");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxInstance"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxInstance.hlsl"), VTXINSTANCE_DECLARATION::Elements, VTXINSTANCE_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_VtxEffect");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxEffect"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxEffect.hlsl"), VTXANIMMODEL_DECLARATION::Elements, VTXANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_VtxCube");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxCube"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxCube.hlsl"), VTXCUBE_DECLARATION::Elements, VTXCUBE_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_VtxNonAnim");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNonAnim"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_VtxNonAnim.hlsl"), VTXNONANIMMODEL_DECLARATION::Elements, VTXNONANIMMODEL_DECLARATION::iNumElements))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Shader_Fire");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_Fire"),
		CShader::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/ShaderFiles/Shader_Fire.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::iNumElements))))
		return E_FAIL;
#pragma endregion

#pragma region VIBuffer
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_VIBuffer_Terrain");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_VIBuffer_Terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Terrain/Height.bmp")))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_VIBuffer_RectInstance");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_RectInstance"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pDeviceContext, 10))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_VIBuffer_PointInstance");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_PointInstance"),
		CVIBuffer_Point_Instance::Create(m_pDevice, m_pDeviceContext, 50))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_VIBuffer_Cube");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;
#pragma endregion

#pragma region Navigation
	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Navigation_Map_Tutorial");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Map_Tutorial"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Dat/Stage01/CellList.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Navigation_Map_GrandYork_Battle_02");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_02"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Dat/Stage08/CellList.dat")))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Navigation_Map_GrandYork_Battle_04");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Map_GrandYork_Battle_04"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Dat/Stage09/CellList.dat")))))
		return E_FAIL;
#pragma endregion

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_ForBoss1Level()
{
	m_isFinished = false;

	if (FAILED(Loading_Boss1_Model()))
		return E_FAIL;

	if (FAILED(Loading_Boss1_Texture()))
		return E_FAIL;

	if (FAILED(Loading_Boss1_Object()))
		return E_FAIL;

	if (FAILED(Loading_Boss1_Components()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss1_Model()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss1_Texture()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss1_Object()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss1_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Navigation_Map_Balseph");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Map_Balseph"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Dat/Stage03/CellList.dat")))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_ForBoss2Level()
{
	m_isFinished = false;

	if (FAILED(Loading_Boss2_Model()))
		return E_FAIL;

	if (FAILED(Loading_Boss2_Texture()))
		return E_FAIL;

	if (FAILED(Loading_Boss2_Object()))
		return E_FAIL;

	if (FAILED(Loading_Boss2_Components()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_etc : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss2_Model()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss2_Texture()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss2_Object()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Boss2_Components()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_Component_Navigation_Map_FireAvatar");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Navigation_Map_FireAvatar"),
		CNavigation::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Dat/Stage04/CellList.dat")))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_ForTestLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Test_Model()))
		return E_FAIL;

	if (FAILED(Loading_Test_Texture()))
		return E_FAIL;

	if (FAILED(Loading_Test_Object()))
		return E_FAIL;

	if (FAILED(Loading_Tutorial_Components()))
		return E_FAIL;


	lstrcpy(m_szLoading, L"Loader_etc : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_etc::Loading_Test_Model()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Test_Texture()
{
	return S_OK;
}

HRESULT CLoader_etc::Loading_Test_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_etc : Prototype_GameObject_Camera_Default");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Camera_Default"),
		CCamera_Default::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_etc::Loading_Test_Component()
{
	return E_NOTIMPL;
}

CLoader_etc* CLoader_etc::Create(ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut, LEVEL eLevel)
{
	CLoader_etc* pInstance = new CLoader_etc(pDeviceOut, pDeviceContextOut);
	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSG_BOX(TEXT("Failed to Created CLoader_etc"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader_etc::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}