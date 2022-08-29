#pragma once
#include "Client_Defines.h"
#include "UI.h"
#include "GameInstance.h"
BEGIN(Client)

class BossHpBar final : public CUI
{
public:
	BossHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	BossHpBar(const BossHpBar& rhs);
	virtual ~BossHpBar() =default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void SetMonsterHPGone(int index) { m_monsterHPindex = index; }
	bool GageOut() { return m_isZero; }

public:
	typedef struct MonsterUIBarDESC
	{
		UIINFO uiinfo;
		int kind;
	}MONSTERUIBARDESC;
private:
	bool m_isZero = false;
	int m_monsterHPindex = 0;
	int m_kindnum = 0;

	_float m_percent = 0;
	_float m_hp = 100;
	_float m_Redbar = 1.f;

private:
	HRESULT SetUp_Texture();
public:
	static BossHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

};

END