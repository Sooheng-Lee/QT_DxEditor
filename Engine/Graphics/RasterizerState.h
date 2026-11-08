#pragma once
enum RasterizerType
{
	RT_SOLID,
	RT_WIREFRAME,
};

class RasterizerState
{
public:
	RasterizerState();
	~RasterizerState();
	void CreateState(const RasterizerType& type);
	void Render();

private:
	ComPtr<ID3D11RasterizerState> _state;
};

