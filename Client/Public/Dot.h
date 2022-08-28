#pragma once

#include "Client_Defines.h"
#include "GameObject.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

BEGIN(Client)

class CDot : public CGameObject
{
public:
	typedef struct _tagDotDesc {
		_float3 fPos;
	}DotDesc;

private:
	CDot(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	CDot(const CDot& rhs);
	virtual ~CDot() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual	HRESULT NativeConstruct(void* pArg);
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render(void);

private:
	HRESULT SetUp_Components(void* pArg);


private:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	class CRenderer*	m_pRendererCom = nullptr;
	class CCollider*	m_pSphereCom = nullptr;

public:
	static CGameObject* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END