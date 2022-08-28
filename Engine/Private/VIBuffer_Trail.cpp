#include "..\Public\VIBuffer_Trail.h"
#include"GameInstance.h"

CVIBuffer_Trail::CVIBuffer_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Trail::CVIBuffer_Trail(const CVIBuffer_Trail & rhs)
	: CVIBuffer(rhs)
{
}

HRESULT CVIBuffer_Trail::NativeConstruct_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer_Trail::NativeConstruct(void* pArg)
{
#pragma region VERTEX_BUFFER
	m_iStride = sizeof(VTXTEX);
	m_iNumVertices = 2 * (m_iLength + 1);
	m_iNumVBuffers = 1;

	ZeroMemory(&m_VBDesc, sizeof(D3D11_BUFFER_DESC));
	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.StructureByteStride = m_iStride;
	m_VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBDesc.MiscFlags = 0;

	VTXTEX* pVertices = new VTXTEX[m_iNumVertices];

	for (_uint i = 0; i < m_iNumVertices; ++i) 
	{
		pVertices[0].vPosition = _float3(5.f, 5.f, 5.f);
	}
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		pVertices[0].vTexUV = _float2(0.f, 0.f);
	}

	ZeroMemory(&m_VBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_VBSubResourceData.pSysMem = pVertices;

	if (FAILED(__super::Create_VertexBuffer()))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
#pragma endregion

#pragma region INDEX_BUFFER

	m_iPrimitiveIndicesSize = sizeof(FACELISTINDICES16);
	m_iNumPrimitive = 2 * m_iLength;
	m_iNumIndicesPerPrimitive = 3;
	m_eFormat = DXGI_FORMAT_R16_UINT;
	m_eTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	ZeroMemory(&m_IBDesc, sizeof(D3D11_BUFFER_DESC));
	m_IBDesc.ByteWidth = m_iPrimitiveIndicesSize * m_iNumPrimitive;
	m_IBDesc.StructureByteStride = 0;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;

	FACELISTINDICES16* pIndices = new FACELISTINDICES16[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(FACELISTINDICES16) * m_iNumPrimitive);

	for (_uint i = 0; i < 100; i += 2) 
	{
		pIndices[i]._0 = i + 3;
		pIndices[i]._1 = i + 1;
		pIndices[i]._2 = i;

		pIndices[i + 1]._0 = i + 2;
		pIndices[i + 1]._1 = i + 3;
		pIndices[i + 1]._2 = i;
	}

	ZeroMemory(&m_IBSubResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	m_IBSubResourceData.pSysMem = pIndices;

	if (FAILED(Create_IndexBuffer()))
		return E_FAIL;
#pragma endregion

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_Trail::Update(_float4x4* Mat)
{
	if (nullptr == m_pDeviceContext)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CModel* pModelCom = (CModel*)pGameInstance->Get_Component(0, TEXT("Layer_Player"), TEXT("Com_ModelSKL"), 0);

	_matrix		BoneMatrixStart = XMLoadFloat4x4(pModelCom->Get_CombinedTransformationMatrix("KK_R"));
	_matrix		BoneMatrixEnd = XMLoadFloat4x4(pModelCom->Get_CombinedTransformationMatrix("EX_SWO_00_R"));
	
	BoneMatrixStart.r[0] = XMVector3Normalize(BoneMatrixStart.r[0]);
	BoneMatrixStart.r[1] = XMVector3Normalize(BoneMatrixStart.r[1]);
	BoneMatrixStart.r[2] = XMVector3Normalize(BoneMatrixStart.r[2]);
	BoneMatrixEnd.r[0] = XMVector3Normalize(BoneMatrixEnd.r[0]);
	BoneMatrixEnd.r[1] = XMVector3Normalize(BoneMatrixEnd.r[1]);
	BoneMatrixEnd.r[2] = XMVector3Normalize(BoneMatrixEnd.r[2]);

	BoneMatrixStart = BoneMatrixStart * pModelCom->Get_PivotMatrix();
	BoneMatrixEnd = BoneMatrixEnd * pModelCom->Get_PivotMatrix();


	_float4x4 LowMatrix, HightMatrix;

	XMStoreFloat4x4(&LowMatrix, BoneMatrixStart);
	XMStoreFloat4x4(&HightMatrix, BoneMatrixEnd);

	D3D11_MAPPED_SUBRESOURCE			SubResource;

	m_pDeviceContext->Map(m_pVB, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);



	m_TargetMatrix = BoneMatrixStart;

	if (m_iStart == false) {
		m_iStart = true;

		_vector			vPosition;
		_vector			vLow = XMVectorSet(m_vLocalLow.x, m_vLocalLow.y, m_vLocalLow.z, 1.f);

		vLow = XMVector3TransformCoord(vLow, m_TargetMatrix);
		vPosition = vLow;

		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iCount].vPosition, vPosition);

		_vector			vHigh = XMVectorSet(m_vLocalHigh.x, m_vLocalHigh.y, m_vLocalHigh.z, 1.f);
		vHigh = XMVector3TransformCoord(vHigh, m_TargetMatrix);
		vPosition = vHigh;

		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iCount + 1].vPosition, vPosition);

		m_iCount += 2;
	}

	if (m_iNumPrimitive <= m_iCount)
	{
		_uint iRemoveCount = m_iCount / 40;
		m_iCount -= iRemoveCount;
		for (_uint i = 0; i < m_iCount; i += 2) {
			((VTXTEX*)SubResource.pData)[i].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i].vPosition;
			((VTXTEX*)SubResource.pData)[i + 1].vPosition = ((VTXTEX*)SubResource.pData)[iRemoveCount + i + 1].vPosition;
		}

	}
	else {
		m_iCatmullRomIndex[0] = m_iCatmullRomIndex[1];
		m_iCatmullRomIndex[1] = m_iCatmullRomIndex[2];
	}

	vector<_float3> vPos;
	for (_uint i = 0; i < m_iNumVertices; ++i) {
		vPos.push_back(((VTXTEX*)SubResource.pData)[i].vPosition);
	}
	vector<_float2> vUV;
	for (_uint i = 0; i < m_iNumVertices; ++i) {
		vUV.push_back(((VTXTEX*)SubResource.pData)[i].vTexUV);
	}

	m_iEndIndex = m_iCatmullRomCount * 2 + m_iCount;
	m_iCatmullRomIndex[2] = m_iEndIndex - 2;
	m_iCatmullRomIndex[3] = m_iEndIndex;

	((VTXTEX*)SubResource.pData)[m_iEndIndex - 2].vPosition = ((VTXTEX*)SubResource.pData)[m_iCount - 2].vPosition;
	((VTXTEX*)SubResource.pData)[m_iEndIndex - 1].vPosition = ((VTXTEX*)SubResource.pData)[m_iCount - 1].vPosition;

	_vector			vPosition;
	_vector			vLow = XMVectorSet(m_vLocalLow.x, m_vLocalLow.y, m_vLocalLow.z, 1.f);
	vLow = XMVector3TransformCoord(vLow, m_TargetMatrix);
	vPosition = vLow;

	XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iEndIndex].vPosition, vPosition);

	_vector			vHigh = XMVectorSet(m_vLocalHigh.x, m_vLocalHigh.y, m_vLocalHigh.z, 1.f);
	vHigh = XMVector3TransformCoord(vHigh, m_TargetMatrix);
	vPosition = vHigh;

	XMStoreFloat3(&((VTXTEX*)SubResource.pData)[m_iEndIndex + 1].vPosition, vPosition);

	for (_uint i = 0; i < m_iCatmullRomCount; ++i) {
		_uint index = i * 2 + m_iCount - 2;

		_float fWeight = _float(i + 1) / (m_iCatmullRomCount + 1);

		_vector Pos = XMVectorCatmullRom(XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[0]].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[1]].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[2]].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[3]].vPosition),
			fWeight);
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[index].vPosition, Pos);

		Pos = XMVectorCatmullRom(XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[0] + 1].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[1] + 1].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[2] + 1].vPosition),
			XMLoadFloat3(&((VTXTEX*)SubResource.pData)[m_iCatmullRomIndex[3] + 1].vPosition),
			fWeight);
		XMStoreFloat3(&((VTXTEX*)SubResource.pData)[index + 1].vPosition, Pos);
	}
	m_iCount = m_iEndIndex + 2;

	for (_uint i = 0; i < m_iCount; i += 2) 
	{
		XMStoreFloat2(&((VTXTEX*)SubResource.pData)[i].vTexUV, XMVectorSet((_float)i / ((_float)m_iCount - 2), 1.f, 0.f, 0.f));
		XMStoreFloat2(&((VTXTEX*)SubResource.pData)[i + 1].vTexUV, XMVectorSet((_float)i / ((_float)m_iCount - 2), 0.f, 0.f, 0.f));
	}

	m_pDeviceContext->Unmap(m_pVB, 0);
	return S_OK;
}

CVIBuffer_Trail* CVIBuffer_Trail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(pDevice, pDeviceContext);
	if (FAILED(pInstance->NativeConstruct_Prototype()))
	{
		MSG_BOX(L"Failed To CVIBuffer_Trail : Create : pInstance->NativeConstruct_Prototype");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Trail::Clone(void* pArg)
{
	CVIBuffer_Trail* pInstance = new CVIBuffer_Trail(*this);
	if (FAILED(pInstance->NativeConstruct(pArg)))
	{
		MSG_BOX(L"Failed To CVIBuffer_Trail : Clone : pInstance->NativeConstruct");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CVIBuffer_Trail::Free()
{
	__super::Free();
}