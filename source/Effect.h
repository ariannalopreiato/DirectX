#pragma once
class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);

	~Effect();

	ID3DX11Effect*& GetEffect() { return m_pEffect; }
	ID3DX11EffectTechnique*& GetTechnique() { return m_pTechnique; }
	ID3D11InputLayout*& GetInputLayout() { return m_pInputLayout; }
	
private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique;
	ID3D11InputLayout* m_pInputLayout;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};

