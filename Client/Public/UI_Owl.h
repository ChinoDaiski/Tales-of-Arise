#pragma once

#include "Client_Defines.h"
#include "UI.h"
#include "GameInstance.h"

BEGIN(Client)

class CUI_Owl final : public CUI
{
private:
	CUI_Owl(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CUI_Owl(const CUI_Owl& rhs);
	virtual ~CUI_Owl() = default;
public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTexture* m_pTextureCom_OwlNameBase = nullptr;
	CTexture* m_pTextureCom_OwlScriptBase = nullptr;
	CTexture* m_pTextureCom_OwlInteraction = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ConstantTable();

	HRESULT SetUp_Texture();

public:
	void Set_InRange(_bool bInRange) { m_bInRange = bInRange; }

private:
	_bool m_bInteractWithPlayer = false;	// 플레이어와 상호작용을 하는지에 대한 여부, 특정 키를 눌러 활성화된다.
	_bool m_bInRange = false;				// 플레이어와의 거리가 일정 이하일 경우 true 아닐경우 false이다.
	_float m_fDistance = 0.f;				// 플레이어

public:
	static CUI_Owl* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END