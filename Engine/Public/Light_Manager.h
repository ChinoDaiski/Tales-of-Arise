#pragma once

#include "Base.h"
#include "Light.h"

BEGIN(Engine)

class CLight_Manager final : public CBase
{
	DECLARE_SINGLETON(CLight_Manager)
public:
	CLight_Manager();
	virtual ~CLight_Manager() = default;

public:
	const LIGHTDESC* Get_LightDesc(_uint iIndex);

public:
	HRESULT NativeConstruct(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT Add_Lights(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const LIGHTDESC& LightDesc);
	HRESULT Delete_Lights(CLight * pLight);
	HRESULT Clear_Lights(void);
	HRESULT Render_Lights();

	list<class CLight*>* Get_Lights(void) { return &m_Lights; }
	void Set_LightDesc(_uint iIndex, LIGHTDESC LightDesc) {
		list<class CLight*>::iterator iter = m_Lights.begin();

		for (_uint i = 0; i < iIndex; ++i)
			++iter;

		(*iter)->Set_LightDesc(LightDesc);
	}

private:
	list<class CLight*>				m_Lights;
	typedef list<class CLight*>		LIGHTS;

private:
	class CVIBuffer_Rect*			m_pVIBuffer = nullptr;
	class CShader*					m_pShader = nullptr;

	_float4x4			m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;


public:
	virtual void Free() override;
};

END