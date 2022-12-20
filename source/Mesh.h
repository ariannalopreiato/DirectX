#pragma once
#include "ColorRGB.h"
#include "Math.h"

using namespace dae;

struct Vertex_PosCol
{
	Vector3 position;
	ColorRGB color;
};

class Effect;
class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices);
	void Render(ID3D11DeviceContext* pDeviceContext);
	void SetMatrix(float* matrix);
	~Mesh();

private:
	uint32_t m_NumIndices{};
	ID3D11Buffer* m_pVertexBuffer{};
	ID3D11Buffer* m_pIndexBuffer{};
	Effect* m_pEffect{};
};