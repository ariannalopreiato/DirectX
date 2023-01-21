#pragma once
#include "ColorRGB.h"
#include "Math.h"
#include "Texture.h"

using namespace dae;

struct Vertex_PosCol
{
	Vector3 position;
	ColorRGB color;
	Vector2 uv;
	Vector3 normal;
	Vector3 tangent;
};

enum class TechniqueType
{
	linear, point, anisotropic
};

class Effect;
class CompleteEffect;
class FlatEffect;
class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices, Effect* pEffect);
	void Render(ID3D11DeviceContext* pDeviceContext);
	void SetViewProjMatrix(float* matrix);
	void SetWorldMatrix(float* matrix);
	void SetInverseMatrix(float* matrix);
	void SetMaps(Texture* pDiffuseTexture, Texture* pNormal = nullptr, Texture* pSpecular = nullptr, Texture* pGlossiness = nullptr);
	~Mesh();
	TechniqueType m_CurrentTechnique = TechniqueType::point;


private:
	uint32_t m_NumIndices{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	Effect* m_pEffect{};
};