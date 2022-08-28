#include "stdafx.h"
#include "..\Public\Level_Test.h"
#include "GameInstance.h"
#include "Camera_Default.h"

#include "MapObject.h"

CLevel_Test::CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	:CLevel(pDevice, pDeviceContext)
{
}

HRESULT CLevel_Test::NativeConstruct()
{
	if (FAILED(__super::NativeConstruct()))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : NativeConstruct : NativeConstruct", E_FAIL);

	if (FAILED(Ready_Lights()))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : NativeConstruct : Ready_Lights", E_FAIL);


	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

	//if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Boar(TEXT("Layer_Boar"))))
	//	return E_FAIL;

	if (FAILED(Ready_Map("../Bin/Resources/Dat/Stage08/", "MeshList.dat", XMVectorSet(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;

	return S_OK;
}

void CLevel_Test::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

HRESULT CLevel_Test::Render()
{
	if (FAILED(__super::Render()))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : Render : Render", E_FAIL);

	SetWindowText(g_hWnd, TEXT("Test"));

	return S_OK;
}

HRESULT CLevel_Test::Ready_Lights()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	LIGHTDESC LightDesc;
	ZeroMemory(&LightDesc, sizeof(LIGHTDESC));

	LightDesc.eType = tagLightDesc::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);
	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(0.4f, 0.4f, 0.4f, 1.f);
	LightDesc.vSpecular = _float4(0.f, 0.f, 0.f, 0.f);
	if (FAILED(pGameInstance->Add_Lights(m_pDevice, m_pDeviceContext, LightDesc)))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : Ready_Lights : Add_Lights", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Camera(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	CCamera::CAMERADESC CameraDesc;
	ZeroMemory(&CameraDesc, sizeof(CCamera::CAMERADESC));

	CameraDesc.eType = CCamera::CAMERA;
	CameraDesc.vEye = _float3(0.f, 10.f, -15.f);
	CameraDesc.vAt = _float3(0.f, 0.f, 0.f);
	CameraDesc.vAxisY = _float3(0.f, 1.f, 0.f);
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 300.0f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinCX / g_iWinCY;

	if (nullptr == pGameInstance->Add_GameObjectToLayer(LEVEL_STATIC, pLayerTag, TEXT("Prototype_GameObject_Camera_Default"), &CameraDesc))
		MSG_CHECK_RETURN(L"Failed To CLevel_Test : Ready_Layer_Camera : Add_GameObjectToLayer(Prototype_GameObject_Camera_Default)", E_FAIL);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Layer_Player(const _tchar * pLayerTag)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CLevel_Test::Ready_Map(const char * pModelFilePath, const char * pModelFileName, _vector vCenterPos)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	char szFullPath[MAX_PATH] = "";

	strcpy_s(szFullPath, pModelFilePath);
	strcat_s(szFullPath, pModelFileName);

	TCHAR szCurPath[MAX_PATH] = TEXT("");
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, szFullPath, (int)strlen(szFullPath), szCurPath, MAX_PATH);

	// ������ ����.
	HANDLE		hFile = CreateFile(szCurPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD		dwByte = 0;
	DWORD		dwStrByte = 0;

	CMapObject::MAPOBJDesc Desc;

	while (true)
	{
		// ���ڿ� �޸��� ũ�⸦ ����
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);

		// �о�� �޸��� ũ�⸸ŭ ���ڿ��� �Ҵ��ϰ�, �ش� ���ڿ��� �о��
		TCHAR*	pName = new TCHAR[dwStrByte];
		ReadFile(hFile, pName, dwStrByte, &dwByte, nullptr);

		// �߸����� ��� �ش� �޸� ���� �� ���߱�
		if (0 == dwByte)
		{
			delete[]pName;
			break;
		}

		// Desc�� ���� �ֱ�
		Desc.pPrototype_ObjectName = pName;

		// Collider ���� ����
		Desc.fColliderSize = _float3(10.f, 10.f, 10.f);

		// ��� ������ ����
		ReadFile(hFile, &Desc.TransformMatrix, sizeof(_float4x4), &dwByte, nullptr);

		_float4x4 matWorld4x4;
		XMStoreFloat4x4(&matWorld4x4, XMMatrixIdentity());

		_float3 fPos;
		XMStoreFloat3(&fPos, vCenterPos);
		memcpy(&matWorld4x4.m[CTransform::STATE_POSITION], &fPos, sizeof(_float3));

		// ���Ͽ��� ���� ���� ���ڷ� ���� ���� ��ġ�� ���Ͽ� ������Ʈ���� �ش� ��ġ�� �̵���Ų��.
		XMStoreFloat4x4(&Desc.TransformMatrix, XMMatrixMultiply(XMLoadFloat4x4(&Desc.TransformMatrix), XMLoadFloat4x4(&matWorld4x4)));



		// �� ���� Ȯ��
		ReadFile(hFile, &Desc.m_bMap, sizeof(_bool), &dwByte, nullptr);

		Desc.eLevel = LEVEL_STATIC;

		if (!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02")) ||
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_02_NaviMap")) ||
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_04")) ||
			!_tcscmp(Desc.pPrototype_ObjectName, TEXT("Prototype_Component_Model_Map_GrandYork_Battle_04_NaviMap"))
			) {
			Desc.eLevel = LEVEL_STATIC;
		}

		// �� ������Ʈ ����
		CMapObject* pMapObject = dynamic_cast<CMapObject*>(pGameInstance->Add_GameObjectToLayer(LEVEL_TEST, LAYER_MAP_TEST, TEXT("Prototype_GameObject_MapObject"), &Desc));
	}

	// ������ �ݴ´�.
	CloseHandle(hFile);

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CLevel_Test* CLevel_Test::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CLevel_Test* pInstance = new CLevel_Test(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct()))
	{
		MSG_BOX(L"Failed To CLevel_Test : Create : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Test::Free()
{
	__super::Free();
}