#pragma once
#include "Camera.h"
#include "Mesh.h"
#include "Texture.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11Effect*& GetEffect() { return m_pEffect; }
	ID3DX11EffectTechnique*& GetPointTechnique() { return m_pPointTechnique; }
	ID3DX11EffectTechnique*& GetLinearTechnique() { return m_pLinearTechnique; }
	ID3DX11EffectTechnique*& GetAnisotropicTechnique() { return m_pAnisotropicTechnique; }
	ID3D11InputLayout*& GetInputLayout() { return m_pInputLayout; }
	void SetMatrix(float* matrix);
	void SetDiffuseMap(Texture* pDiffuseTexture);

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pPointTechnique;
	ID3DX11EffectTechnique* m_pLinearTechnique;
	ID3DX11EffectTechnique* m_pAnisotropicTechnique;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;

	//world view projection matrix variable
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	//virtual void BuildInputLayout() = 0;
	//virtual void LoadEffectVariable() = 0;
};