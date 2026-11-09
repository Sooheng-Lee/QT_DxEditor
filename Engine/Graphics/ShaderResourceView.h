#pragma once
class ShaderResourceView
{
public:
	ShaderResourceView();
	~ShaderResourceView();
	void CreateShaderResourceView(const UINT& width, const UINT& height, const uint8_t* pixels);
	void LoadFromFile(const std::string& filePath);
	void Render();
private:
	void CreateSampler();
	ComPtr<ID3D11ShaderResourceView> _srView;
	ComPtr<ID3D11SamplerState> _sampler;
};

