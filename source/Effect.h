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
	void SetViewProjMatrix(float* matrix);
	void SetWorldMatrix(float* matrix);
	void SetInverseMatrix(float* matrix);
	void SetMaps(Texture* pDiffuseTexture, Texture* pNormal, Texture* pSpecular, Texture* pGlossiness);

	virtual void BuildInputLayout(ID3D11Device* pDevice) {};
	virtual void LoadEffectVariable() {};

protected:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pPointTechnique;
	ID3DX11EffectTechnique* m_pLinearTechnique;
	ID3DX11EffectTechnique* m_pAnisotropicTechnique;

	ID3D11InputLayout* m_pInputLayout;

	//world view projection matrix variable
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable{};
	ID3DX11EffectMatrixVariable* m_pMatWorldProjVariable{};
	ID3DX11EffectMatrixVariable* m_pViewInverseVariable{};
	
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable{};
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable{};

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};

class CompleteEffect : public Effect
{

public:
	CompleteEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

private:
	void BuildInputLayout(ID3D11Device* pDevice);
	void LoadEffectVariable();
};

class FlatEffect : public Effect
{
public:
	FlatEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

private:
	void BuildInputLayout(ID3D11Device* pDevice);
	void LoadEffectVariable();
};