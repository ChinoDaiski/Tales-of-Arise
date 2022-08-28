#include "stdafx.h"
#include "..\Public\Loader_MapObject.h"
#include "GameInstance.h"

#include "Owl.h"
#include "UI_Owl.h"
#include "WayPoint.h"
#include "WayPoint_Pos.h"
#include "Balseph_Stair.h"

CLoader_MapObject::CLoader_MapObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:m_pDevice(pDevice), m_pDeviceContext(pDeviceContext)
{
	Safe_AddRef(m_pDeviceContext);
	Safe_AddRef(m_pDevice);
}

CRITICAL_SECTION CLoader_MapObject::Get_CriticalSection()
{
	return m_CriticalSection;
}

LEVEL CLoader_MapObject::Get_LevelIndex()
{
	return m_eLevel;
}

_bool CLoader_MapObject::Get_Finished() const
{
	return m_isFinished;
}

const _tchar* CLoader_MapObject::Get_LoadingText() const
{
	return m_szLoading;
}

unsigned int APIENTRY ThreadEntryFunc_MapObject(void* pArg)
{
	CLoader_MapObject* pLoader = (CLoader_MapObject*)pArg;
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
	}
	LeaveCriticalSection(&pLoader->Get_CriticalSection());
	return 0;
}

HRESULT CLoader_MapObject::NativeConstruct(LEVEL eLevel)
{
	m_eLevel = eLevel;
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	InitializeCriticalSection(&m_CriticalSection);

	m_hThread = (HANDLE)_beginthreadex(nullptr, 0, ThreadEntryFunc_MapObject, this, 0, nullptr);
	if (0 == m_hThread)
		return E_FAIL;

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_ForLobbyLevel()
{
	m_isFinished = false;

	lstrcpy(m_szLoading, L"Loader_MapObject : Model");
	if (FAILED(Loading_Lobby_Model()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Texture");
	if (FAILED(Loading_Lobby_Texture()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Object");
	if (FAILED(Loading_Lobby_Object()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Lobby_Model()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Lobby_Texture()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Lobby_Object()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_ForTutorialLevel()
{
	m_isFinished = false;

	if (FAILED(Loading_Tutorial_Model()))
		return E_FAIL;

	if (FAILED(Loading_Tutorial_Texture()))
		return E_FAIL;

	if (FAILED(Loading_Tutorial_Object()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Tutorial_Model()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_Component_Model_Map_GrandYork_Battle_02");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Resources/UMap/Map_GrandYork/", "Map_GrandYork_Battle02.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_Component_Model_Map_GrandYork_Battle_02_NaviMap");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02_NaviMap"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Resources/UMap/Map_GrandYork/", "Map_GrandYork_Battle02_NaviMap.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_Component_Model_Map_GrandYork_Battle_04");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_04"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Resources/UMap/Map_GrandYork/", "Map_GrandYork_Battle04.fbx", PivotMatrix))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_Component_Model_Map_GrandYork_Battle_04_NaviMap");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_04_NaviMap"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM, "../Bin/Resources/UMap/Map_GrandYork/", "Map_GrandYork_Battle04_NaviMap.fbx", PivotMatrix))))
		return E_FAIL;

	// Owl
	PivotMatrix = XMMatrixScaling(0.1f, 0.1f, 0.1f);
	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_Component_Model_Object_Owl");
	if (FAILED(pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Model_Owl_001"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM, "../Bin/Resources/UMap/MapObejct/Owl/", "Owl_001.fbx", PivotMatrix, false))))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("Loader_MapObject : Prototype_Component_Model_DRTHouse"));
	if (FAILED(Loading_LEVEL_MapTool_Model_DRTHouse(LEVEL_TUTORIAL)))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("Loader_MapObject : Prototype_Component_Model_BrokenHouse"));
	if (FAILED(Loading_LEVEL_MapTool_Model_BrokenHouse(LEVEL_TUTORIAL)))
		return E_FAIL;


	lstrcpy(m_szLoading, TEXT("Loader_MapObject : Prototype_Component_Model_TorchLight"));
	if (FAILED(Loading_LEVEL_MapTool_Model_TorchLight(LEVEL_STATIC)))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("Loader_MapObject : Prototype_Component_Model_Furniture"));
	if (FAILED(Loading_LEVEL_MapTool_Model_Furniture(LEVEL_STATIC)))
		return E_FAIL;

	lstrcpy(m_szLoading, TEXT("Loader_MapObject : Prototype_Component_Model_Anim_Objects"));
	if (FAILED(Loading_LEVEL_MapTool_Model_Anim_Objects(LEVEL_STATIC)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Tutorial_Texture()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Tutorial_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_GameObject_Owl");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Owl"),
		COwl::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_GameObject_UI_Owl");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_UI_Owl"),
		CUI_Owl::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_GameObject_WayPoint");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WayPoint"),
		CWayPoint::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_GameObject_WayPoint");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_WayPoint_Pos"),
		CWayPoint_Pos::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_ForBoss1Level()
{
	m_isFinished = false;

	lstrcpy(m_szLoading, L"Loader_MapObject : Model");
	if (FAILED(Loading_Boss1_Model()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Texture");
	if (FAILED(Loading_Boss1_Texture()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Object");
	if (FAILED(Loading_Boss1_Object()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Boss1_Model()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Boss1_Texture()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Boss1_Object()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	lstrcpy(m_szLoading, L"Loader_MapObject : Prototype_GameObject_Balseph_Stair");
	if (FAILED(pGameInstance->Add_Prototype(TEXT("Prototype_GameObject_Balseph_Stair"),
		CBalseph_Stair::Create(m_pDevice, m_pDeviceContext))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_ForBoss2Level()
{
	m_isFinished = false;

	lstrcpy(m_szLoading, L"Loader_MapObject : Model");
	if (FAILED(Loading_Boss2_Model()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Texture");
	if (FAILED(Loading_Boss2_Texture()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Object");
	if (FAILED(Loading_Boss2_Object()))
		return E_FAIL;

	lstrcpy(m_szLoading, L"Loader_MapObject : Finish");
	m_isFinished = true;

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Boss2_Model()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Boss2_Texture()
{
	return S_OK;
}

HRESULT CLoader_MapObject::Loading_Boss2_Object()
{
	return S_OK;
}

CLoader_MapObject* CLoader_MapObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, LEVEL eLevel)
{
	CLoader_MapObject*	pInstance = new CLoader_MapObject(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct(eLevel)))
	{
		MSG_BOX(L"Failed To CLoader_MapObject : Create : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLoader_MapObject::Free()
{
	WaitForSingleObject(m_hThread, INFINITE);
	DeleteCriticalSection(&m_CriticalSection);
	DeleteObject(m_hThread);
	CloseHandle(m_hThread);

	Safe_Release(m_pDeviceContext);
	Safe_Release(m_pDevice);
}

HRESULT CLoader_MapObject::Loading_LEVEL_MapTool_Model_BrokenHouse(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			PivotMatrix;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_BrokenHouse01_A"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/BrokenHouse/",
			"BrokenHouse01_A.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_BrokenHouse01_B"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/BrokenHouse/",
			"BrokenHouse01_B.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_BrokenHouse01_C"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/BrokenHouse/",
			"BrokenHouse01_C.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_BrokenHouse02_A"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/BrokenHouse/",
			"BrokenHouse02_A.fbx", PivotMatrix))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_LEVEL_MapTool_Model_DRTHouse(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			PivotMatrix;

	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_BrokenHouse02_B"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"BrokenHouse02_B.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_BrokenHouse02_C"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"BrokenHouse02_C.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse02"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse02.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse03"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse03.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse05"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse05.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse06"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse06.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse07"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse07.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse08"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse08.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse09"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse09.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse10"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse10.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse11"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse11.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_DRTHouse12"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/DRTHouse/",
			"DRTHouse12.fbx", PivotMatrix))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_LEVEL_MapTool_Model_TorchLight(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			PivotMatrix;

	// 횟불
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Torchlight_Balseph"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Torchlight_Balseph.fbx", PivotMatrix))))
		return E_FAIL;

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_GRN_Prop_Torch"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/Cave/",
			"GRN_Prop_Torch.fbx", PivotMatrix))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_LEVEL_MapTool_Model_Furniture(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			PivotMatrix;

	// 가구
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture02"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture02.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture03"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture03.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture04"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture04.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture05"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture05.fbx", PivotMatrix))))
		return E_FAIL;
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture07"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture07.fbx", PivotMatrix))))
		return E_FAIL;

	// 0.003f * 0.65f = 0.00195f;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture08_Picture"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture08_Picture.fbx", PivotMatrix))))
		return E_FAIL;


	// 0.003f * 0.4f = 0.0012f;
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);
	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Furniture09"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_NONANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Furniture09.fbx", PivotMatrix))))
		return E_FAIL;
	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLoader_MapObject::Loading_LEVEL_MapTool_Model_Anim_Objects(LEVEL eLevel)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix			PivotMatrix;

	// 가구
	PivotMatrix = XMMatrixScaling(0.01f, 0.01f, 0.01f);

	if (FAILED(pGameInstance->Add_Prototype(eLevel, TEXT("Prototype_Component_Model_Stair_Object"),
		CModel::Create(m_pDevice, m_pDeviceContext, CModel::TYPE_ANIM,
			"../Bin/Resources/Map/D03_GRN/",
			"Map_Balseph_Stair.fbx", PivotMatrix, false))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}
