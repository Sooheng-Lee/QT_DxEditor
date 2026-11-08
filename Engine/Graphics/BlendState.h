#pragma once
enum class BlendType
{
	BT_OPAQUE,
	BT_ALPHA
};

class BlendState
{
public:
	BlendState();
	~BlendState();
	void CreateBlendState(const BlendType& type);
	void Render();
private:
	ComPtr<ID3D11BlendState> _state;
	BlendType _type;
};

