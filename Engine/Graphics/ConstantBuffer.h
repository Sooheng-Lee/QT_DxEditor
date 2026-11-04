#pragma once
enum ERenderType
{
	RT_VERTEX,
	RT_PIXEL,
};

template <typename T>
class ConstantBuffer
{
public:
	ConstantBuffer() = default;
	~ConstantBuffer();
	void CreateConstantBuffer(const ERenderType& type, const UINT& slot);
	void Update(const T& data);
	void Render();

private:
	ComPtr<ID3D11Buffer> _buffer;
	ERenderType _renderType;
	UINT _slot = 0;
};

template<typename T>
inline ConstantBuffer<T>::~ConstantBuffer()
{
}

template<typename T>
inline  void ConstantBuffer<T>::Update(const T& data)
{
	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = Graphics::GetInstance()->GetContext()->Map(_buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (SUCCEEDED(hr))
	{
		memcpy(resource.pData, &data, sizeof(T));
		Graphics::GetInstance()->GetContext()->Unmap(_buffer.Get(), 0);
	}
}

template<typename T>
inline  void ConstantBuffer<T>::Render()
{
	switch (_renderType)
	{
	case ERenderType::RT_VERTEX:
		Graphics::GetInstance()->GetContext()->VSSetConstantBuffers(_slot, 1, _buffer.GetAddressOf());
		break;
	case ERenderType::RT_PIXEL:
		Graphics::GetInstance()->GetContext()->PSSetConstantBuffers(_slot, 1, _buffer.GetAddressOf());
		break;
	}
}

template<typename T>
inline void ConstantBuffer<T>::CreateConstantBuffer(const ERenderType& type, const UINT& slot)
{
	_renderType = type;
	_slot = slot;
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(T);
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateBuffer(&desc, nullptr, _buffer.GetAddressOf());
	CHECK(hr);
}
