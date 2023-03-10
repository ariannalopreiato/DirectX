#include "pch.h"
#include "Mesh.h"
#include "Effect.h"

Mesh::Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices, Effect* pEffect)
{
	m_pEffect = pEffect;

	//create vertex buffer
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex_PosCol) * static_cast<uint32_t>(vertices.size());
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	HRESULT result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
		return;

	//create index buffer
	m_NumIndices = static_cast<uint32_t>(indices.size());
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_NumIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = indices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);

	if (FAILED(result))
		return;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	//1. set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//2. set input layout
	pDeviceContext->IASetInputLayout(m_pEffect->GetInputLayout()); 

	//3. set vertex buffer
	constexpr UINT stride = sizeof(Vertex_PosCol);
	constexpr UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	//4. set index buffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//5. draw
	D3DX11_TECHNIQUE_DESC techDesc{};
	ID3DX11EffectTechnique* technique{};

	switch (m_CurrentTechnique)
	{
	case TechniqueType::linear:
		technique = m_pEffect->GetLinearTechnique(); 
		break;

	case TechniqueType::point:
		technique = m_pEffect->GetPointTechnique();
		break;

	case TechniqueType::anisotropic:
		technique = m_pEffect->GetAnisotropicTechnique();
		break;
	}
	
	technique->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		technique->GetPassByIndex(p)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_NumIndices, 0, 0);
	}
}

void Mesh::SetViewProjMatrix(float* matrix)
{
	m_pEffect->SetViewProjMatrix(matrix);
}

void Mesh::SetWorldMatrix(float* matrix)
{
	m_pEffect->SetWorldMatrix(matrix);
}

void Mesh::SetInverseMatrix(float* matrix)
{
	m_pEffect->SetInverseMatrix(matrix);
}

void Mesh::SetMaps(Texture* pDiffuseTexture, Texture* pNormal, Texture* pSpecular, Texture* pGlossiness)
{
	m_pEffect->SetMaps(pDiffuseTexture, pNormal, pSpecular, pGlossiness);
}

Mesh::~Mesh()
{
	m_pIndexBuffer->Release();
	m_pIndexBuffer = nullptr;

	m_pVertexBuffer->Release();
	m_pVertexBuffer = nullptr;

	delete m_pEffect;
	m_pEffect = nullptr;
}
