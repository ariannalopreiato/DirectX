#pragma once
#include "Camera.h"
#include "Mesh.h"

class Effect
{
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetFile);
	~Effect();

	ID3DX11Effect*& GetEffect() { return m_pEffect; }
	ID3DX11EffectTechnique*& GetTechnique() { return m_pTechnique; }
	ID3D11InputLayout*& GetInputLayout() { return m_pInputLayout; }
	void SetMatrix(const Matrix& matrix);

private:
	ID3DX11Effect* m_pEffect;
	ID3DX11EffectTechnique* m_pTechnique{};
	ID3D11InputLayout* m_pInputLayout{};
	//world view projection matrix variable
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;

	//ID3D11Buffer* m_pCBuffer;
	//ID3D11Buffer* m_pTBuffer;
	//ID3D11UnorderedAccessView* m_pUAV;
	//ID3D11ShaderResourceView* m_pSRV;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);

	//virtual void BuildInputLayout() = 0;
	//virtual void LoadEffectVariable() = 0;
};

//class Effect_PosTex final : public Effect

