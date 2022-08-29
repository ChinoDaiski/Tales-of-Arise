#pragma once
#include "Client_Defines.h"
#include "Level.h"
#include "Battle_Manager.h"
BEGIN(Client)

class CLevel_Test final : public CLevel
{
private:
	CLevel_Test(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Test() = default;
public:
	virtual HRESULT NativeConstruct() override;
	virtual void Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
private:
	HRESULT Ready_Map_Battle02(_vector vCenterPos);
	HRESULT Ready_Layer_Boar(const _tchar * pLayerTag);
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Player(const _tchar* pLayerTag);
	HRESULT Ready_Map(const char * pModelFilePath, const char * pModelFileName, _vector vCenterPos);
public:
	static CLevel_Test* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;

private:
	CBattle_Manager* m_pBattle_Manager = nullptr;
};

END