#include "pch.h"
#include "Effect.h"

Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	m_pEffect = LoadEffect(pDevice, assetFile);

	//Techniques
	m_pPointTechnique = m_pEffect->GetTechniqueByName("PointTechnique");
	if (!m_pPointTechnique->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pLinearTechnique = m_pEffect->GetTechniqueByName("LinearTechnique");
	if (!m_pLinearTechnique->IsValid())
		std::wcout << L"Technique not valid\n";

	m_pAnisotropicTechnique = m_pEffect->GetTechniqueByName("AnisotropicTechnique");
	if (!m_pAnisotropicTechnique->IsValid())
		std::wcout << L"Technique not valid\n";
	//-------------------------------------------------------------------------------------------------

	//Matrices
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

	m_pMatWorldProjVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldProjVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";

	m_pViewInverseVariable = m_pEffect->GetVariableByName("gInverseMatrix")->AsMatrix();
	if (!m_pViewInverseVariable->IsValid())
		std::wcout << L"m_pMatWorldViewProjVariable not valid!\n";
	//-------------------------------------------------------------------------------------------------

	//Maps
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		std::wcout << L"m_pDiffuseMapVariable not valid!\n";
	//-------------------------------------------------------------------------------------------------

	//create vertex layout
	static constexpr uint32_t numElements{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElements]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "COLOR";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "TEXCOORD";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 24;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "NORMAL";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "TANGENT";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//create input layout
	D3DX11_PASS_DESC passDesc{};
	m_pPointTechnique->GetPassByIndex(0)->GetDesc(&passDesc);

	HRESULT result = pDevice->CreateInputLayout(
		vertexDesc,
		numElements,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pInputLayout);

	if (FAILED(result))
		return;
}

Effect::~Effect()
{
	m_pMatWorldViewProjVariable->Release();
	m_pMatWorldViewProjVariable = nullptr;

	m_pMatWorldProjVariable->Release();
	m_pMatWorldProjVariable = nullptr;

	m_pViewInverseVariable->Release();
	m_pViewInverseVariable = nullptr;

	m_pDiffuseMapVariable->Release();
	m_pDiffuseMapVariable = nullptr;

	m_pNormalMapVariable->Release();
	m_pNormalMapVariable = nullptr;

	m_pSpecularMapVariable->Release();
	m_pSpecularMapVariable = nullptr;

	m_pGlossinessMapVariable->Release();
	m_pGlossinessMapVariable = nullptr;

	m_pInputLayout->Release();
	m_pInputLayout = nullptr;

	m_pPointTechnique->Release();
	m_pPointTechnique = nullptr;

	m_pLinearTechnique->Release();
	m_pLinearTechnique = nullptr;

	m_pEffect->Release();
	m_pEffect = nullptr;
}

void Effect::SetViewProjMatrix(float* matrix)
{
	m_pMatWorldViewProjVariable->SetMatrix(matrix);
}

void Effect::SetWorldMatrix(float* matrix)
{
	m_pMatWorldProjVariable->SetMatrix(matrix);
}

void Effect::SetInverseMatrix(float* matrix)
{
	m_pViewInverseVariable->SetMatrix(matrix);
}

ID3DX11Effect* Effect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result;
	ID3D10Blob* pErrorBlob{ nullptr };
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0, pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			const char* pErrors = static_cast<char*>(pErrorBlob->GetBufferPointer());

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); ++i)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else
		{
			std::wstringstream ss;
			ss << "EffectLoader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

void Effect::SetMaps(Texture* pDiffuseTexture, Texture* pNormal, Texture* pSpecular, Texture* pGlossiness)
{
	if (m_pDiffuseMapVariable)
		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetResourceView());

	if (m_pNormalMapVariable)
		m_pNormalMapVariable->SetResource(pNormal->GetResourceView());

	if (m_pSpecularMapVariable)
		m_pSpecularMapVariable->SetResource(pSpecular->GetResourceView());

	if (m_pGlossinessMapVariable)
		m_pGlossinessMapVariable->SetResource(pGlossiness->GetResourceView());
}


//Effect_PosTex::Effect_PosTex(ID3D11Device* pDevice, const std::wstring& assetFile)
//	: Effect(pDevice, assetFile)
//{	
//	m_pEffect = Effect::GetEffect();
//	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
//	if (!m_pDiffuseMapVariable->IsValid())
//	{
//		std::wcout << L"m_pDiffuseMapVariable not valid!\n";
//	}
//}
//
//void Effect_PosTex::SetDiffuseMap(Texture* pDiffuseTexture)
//{
//	if (m_pDiffuseMapVariable)
//		m_pDiffuseMapVariable->SetResource(pDiffuseTexture->GetResourceView());
//}
