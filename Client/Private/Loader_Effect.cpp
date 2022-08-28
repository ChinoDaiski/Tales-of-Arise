#include "stdafx.h"
#include "..\Public\Loader_Effect.h"
#include "GameInstance.h"

#include "Trail.h"

#include "Rect_Effect.h"
#include "Point_Effect.h"
#include "MeshEffect.h"
#include "Effect.h"
#include "FireAvatar_InstanceRect.h"

CLoader_Effect::CLoader_Effect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CRITICAL_SECTION CLoader_Effect::Get_CriticalSection()
{
	return m_CriticalSection;
}

LEVEL CLoader_Effect::Get_LevelIndex()
{
	return m_eLevel;
}

_bool CLoader_Effect::Get_Finished() const
{
	return m_isFinished;
}

const _tchar* CLoader_Effect::Get_LoadingText() const
{
	return m_szLoading;
}

unsigned int APIENTRY ThreadEntryFunc_Effect(void* pArg)
{
	CLoader_Effect* pLoader = (CLoader_Effect*)pArg;
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

HRESULT CLoader_Effect::NativeConstruct(LEVEL eLevel)
{
	m_eLevel = eLevel;
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc_Effect, this, 0, nullptr);
	if (0 == m_hThread)
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : NativeConstruct : m_hThread", E_FAIL);

	return S_OK;
}

HRESULT CLoader_Effect::Loading_ForLobbyLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Lobby_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForLobbyLevel : Loading_Lobby_Model", E_FAIL);

	if (FAILED(Loading_Lobby_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForLobbyLevel : Loading_Lobby_Texture", E_FAIL);

	if (FAILED(Loading_Lobby_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForLobbyLevel : Loading_Lobby_Object", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Effect::Loading_Lobby_Model()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Lobby_Texture()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	//불이펙트
	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_Fire0");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_TUTORIAL, TEXT("Prototype_Component_Texture_Fire0"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/Fire/Fire0(%d).png"), 64))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_Fire0)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_FireEffect0");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireEffect0"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/FireEffect/FireAvatar_Effect0(%d).png"), 36))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_FireEffect0)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_FireEffect1");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireEffect1"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/FireEffect/FireAvatar_Effect1(%d).png"), 36))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_FireEffect1)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_FireEffect2");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireEffect2"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/FireEffect/FireAvatar_Effect2(%d).png"), 36))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_FireEffect2)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_FireEffect3");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireEffect3"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/FireEffect/FireAvatar_Effect3(%d).png"), 36))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_FireEffect3)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_FireEffect4");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireEffect4"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/FireEffect/FireAvatar_Effect4(%d).png"), 36))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_FireEffect4)", E_FAIL);

	//연기 이펙트
	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_FireEffect5");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_FireEffect5"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Sprite/FireEffect/FireAvatar_Effect5(%d).png"), 36))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Lobby_Texture : Add_Prototype(Prototype_Component_Texture_FireEffect5)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Lobby_Object()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_ForTutorialLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Tutorial_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForTutorialLevel : Loading_Tutorial_Model", E_FAIL);

	if (FAILED(Loading_Tutorial_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForTutorialLevel : Loading_Tutorial_Texture", E_FAIL);

	if (FAILED(Loading_Tutorial_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForTutorialLevel : Loading_Tutorial_Object", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Effect::Loading_Tutorial_Model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			PivotMatrix, PivotAxeMatrix;

	PivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f)*XMMatrixRotationX(XMConvertToRadians(90.f));

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Model_Effect1");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Effect1",
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_EFFECT, "../Bin/Resources/Model/Effect/", "0.fbx", PivotMatrix))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Model : Add_Prototype(Prototype_Component_Model_Effect1)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_Effect::Loading_Tutorial_Texture()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_Effect_Mesh");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect_Mesh"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Mesh/Mesh(%d).png"), 421))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_Effect_Mesh)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_Effect");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Effect"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Effect/Effect(%d).png"), 29))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_Effect)", E_FAIL);

	//트레일
	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Texture_Trail");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Trail"),
		CTexture::Create(m_pDevice, m_pDeviceContext, TEXT("../Bin/Resources/Textures/Trail/Trail%d.dds"), 1))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Texture : Add_Prototype(Prototype_Component_Texture_Trail)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Tutorial_Object()
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_GameObject_Rect_Effect");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Rect_Effect"),
		CRect_Effect::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Rect_Effect)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_GameObject_Point_Effect");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Point_Effect"),
		CPoint_Effect::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Point_Effect)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_GameObject_Mesh_Effect");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Mesh_Effect"),
		CMeshEffect::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Mesh_Effect)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_GameObject_Effect");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Effect"),
		CEffect::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Effect)", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_GameObject_Trail");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Trail"),
		CTrail::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Tutorial_Object : Add_Prototype(Prototype_GameObject_Trail)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_Effect::Loading_ForBoss1Level()
{
	m_isFinished = false;

	if (FAILED(Loading_Boss1_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForBoss1Level : Loading_Boss1_Model", E_FAIL);

	if (FAILED(Loading_Boss1_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForBoss1Level : Loading_Boss1_Texture", E_FAIL);

	if (FAILED(Loading_Boss1_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForBoss1Level : Loading_Boss1_Object", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Effect::Loading_Boss1_Model()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Boss1_Texture()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Boss1_Object()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_ForBoss2Level()
{
	m_isFinished = false;

	if (FAILED(Loading_Boss2_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForBoss2Level : Loading_Boss2_Model", E_FAIL);

	if (FAILED(Loading_Boss2_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForBoss2Level : Loading_Boss2_Texture", E_FAIL);

	if (FAILED(Loading_Boss2_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForBoss2Level : Loading_Boss2_Object", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Effect::Loading_Boss2_Model()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Boss2_Texture()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Boss2_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_GameObject_FireInstance");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_FireInstance"),
		CFireAvatar_InstanceRect::Create(m_pDevice, m_pDeviceContext))))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_Boss2_Object : Add_Prototype(Prototype_GameObject_FireInstance)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);
	return S_OK;
}

HRESULT CLoader_Effect::Loading_ForTestLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Test_Model()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForTestLevel : Loading_Test_Model", E_FAIL);

	if (FAILED(Loading_Test_Texture()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForTestLevel : Loading_Test_Texture", E_FAIL);

	if (FAILED(Loading_Test_Object()))
		MSG_CHECK_RETURN(L"Failed To CLoader_Effect : Loading_ForTestLevel : Loading_Test_Object", E_FAIL);

	lstrcpy(m_szLoading, L"Loader_Effect : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_Effect::Loading_Test_Model()
{

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);


	_matrix			PivotMatrix, PivotAxeMatrix;

	PivotMatrix = XMMatrixScaling(3.f, 3.f, 3.f)*XMMatrixRotationX(XMConvertToRadians(90.f));

	lstrcpy(m_szLoading, L"Loader_Effect : Prototype_Component_Model_Effect1");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, L"Prototype_Component_Model_Effect1",
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_EFFECT, "../Bin/Resources/Model/Effect/", "0.fbx", PivotMatrix))))
		return E_FAIL;

	Safe_Release(pGameInstance);


	return S_OK;
}

HRESULT CLoader_Effect::Loading_Test_Texture()
{
	return S_OK;
}

HRESULT CLoader_Effect::Loading_Test_Object()
{
	return S_OK;
}

CLoader_Effect* CLoader_Effect::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eLevel)
{
	CLoader_Effect*	pInstance = new CLoader_Effect(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSG_BOX(L"Failed To CLoader_Effect : Create : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader_Effect::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}