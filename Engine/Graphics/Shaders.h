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
	void CreateVertexBuffer(const std::vector<T>& vertices);
	void CreateIndexBuffer(const std::vector<UINT>& indices);
	void CreateShader(const wchar_t* hlslPath);
	void Render();
	void Draw();

private:
	void CreateInputLayout(const wchar_t* hlslPath);

private:
	std::vector<T> _vertices;
	std::vector<UINT> _indices;
	ComPtr<ID3D11InputLayout> _inputLayout;
	ComPtr<ID3D11Buffer> _vertexBuffer;
	ComPtr<ID3D11Buffer> _indexBuffer;
	ComPtr<ID3D11VertexShader> _shader;
	UINT _stride = 0;
	UINT _offset = 0;
};

class PixelShader final : public ShaderBase
{
public:
	void CreateShader(const wchar_t* hlslPath);
	void Render();
private:
	ComPtr<ID3D11PixelShader> _shader;
};

template<typename T>
inline void VertexShader<T>::CreateInputLayout(const wchar_t* hlslPath)
{
	Graphics::GetInstance()->CompileFromFile(hlslPath, "VS", "vs_5_0", blob);
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	UINT numElements = ARRAYSIZE(inputElements);
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateInputLayout(inputElements, numElements, blob->GetBufferPointer(), 
		blob->GetBufferSize(), _inputLayout.GetAddressOf());
	CHECK(hr);
}

template<typename T>
inline void VertexShader<T>::CreateVertexBuffer(const std::vector<T>& vertices)
{
	_vertices = vertices;
	_stride = sizeof(T);
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = _stride * _vertices.size();
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;
	}
	
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = _vertices.data();
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _vertexBuffer.GetAddressOf());
	CHECK(hr);
}

template<typename T>
inline void VertexShader<T>::CreateIndexBuffer(const std::vector<UINT>& indices)
{
	_indices = indices;
	D3D11_BUFFER_DESC desc = {};
	{
		desc.ByteWidth = sizeof(UINT) * _indices.size();
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = 0;
	}

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = _indices.data();
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, &data, _indexBuffer.GetAddressOf());
	CHECK(hr);
}

template<typename T>
inline void VertexShader<T>::CreateShader(const wchar_t* hlslPath)
{
	CreateInputLayout(hlslPath);
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, _shader.GetAddressOf());
	CHECK(hr);
}

template<typename T>
inline void VertexShader<T>::Render()
{
	Graphics::GetInstance()->GetContext()->IASetInputLayout(_inputLayout.Get());
	Graphics::GetInstance()->GetContext()->IASetVertexBuffers(0, 1, _vertexBuffer.GetAddressOf(), &_stride, &_offset);
	Graphics::GetInstance()->GetContext()->IASetIndexBuffer(_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, _offset);
	Graphics::GetInstance()->GetContext()->VSSetShader(_shader.Get(), nullptr, 0);
}

template<typename T>
inline void VertexShader<T>::Draw()
{
	Graphics::GetInstance()->GetContext()->DrawIndexed(_indices.size(), 0, 0);
}
