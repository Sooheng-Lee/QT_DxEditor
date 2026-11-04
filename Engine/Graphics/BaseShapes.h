#pragma once
class BaseShapes
{
public:
	static void CreateBox(std::vector<VertexDefault>& vertices, std::vector<UINT>& indices, float size = 0.5f, XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	static void CreateSphere(std::vector<VertexDefault>& vertices, std::vector<UINT>& indices, float radius=1.0f, UINT slices = 10, XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
};

