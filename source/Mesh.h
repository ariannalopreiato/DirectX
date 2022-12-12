#pragma once
#include "Effect.h"

struct Vector3
{
	float x;
	float y;
	float z;
};

struct Vertex_PosCol
{
	Vector3 pos;
	Vector3 col;
};

class Mesh
{
public:
	Mesh(ID3D11Device* pDevice, std::vector<Vertex_PosCol> vertices, std::vector<uint32_t> indices);
	void Render(ID3D11DeviceContext* pDeviceContext);
	~Mesh();

private:
	uint32_t m_NumIndices;
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;
	Effect* m_pEffect;
};

