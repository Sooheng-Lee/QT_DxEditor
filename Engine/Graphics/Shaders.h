#pragma once
class ShaderBase
{
public:
	ComPtr<ID3DBlob> GetBlob() const { return blob; }
	virtual void Render() = 0;
protected:
	ComPtr<ID3DBlob> blob;
};

template<typename T>
class VertexShader final : public ShaderBase
{
public:
	VertexShader() {};
	~VertexShader() {};
	void CreateInputLayout(const wchar_t* hlslPath);
	void CreateBuffer(const std::vector<T>& vertices);
	void Render();

private:
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11Buffer> _buffer;
	ComPtr<ID3D11VertexShader> _shader;
	UINT stride = 0;
	UINT offset = 0;
};

template<typename T>
inline void VertexShader<T>::CreateInputLayout(const wchar_t* hlslPath)
{
	Graphics::GetInstance()->CompileFromFile(hlslPath, L"VS", "vs_5_0", blob);
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(inputElements);
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateInputLayout(inputElements, numElements, blob->GetBufferSize(), _inputLayout.GetAddressOf());
	CHECK(hr);
}

template<typename T>
inline void VertexShader<T>::CreateBuffer(const std::vector<T>& vertices)
{
	
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = sizeof(T) * vertices.size();
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
	}
	
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices.data();
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _buffer.GetAddressOf());
	CHECK(hr);
}

template<typename T>
inline void VertexShader<T>::Render()
{
	Graphics::GetInstance()->GetContext()->IASetInputLayout(_inputLayout.Get());
	Graphics::GetInstance()->GetContext()->IASetVertexBuffers(0, 1, _buffer.GetAddressOf(), &stride, &offset);
}