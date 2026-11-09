#include "../pch.h"
#include "ShaderResourceView.h"
#include <filesystem>
#include <DirectXTex.h>
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "ole32.lib")

namespace
{
	void RequireSuccess(HRESULT hr, const char* message)
	{
		if (FAILED(hr)) throw std::runtime_error(message);
	}

	struct ComScope
	{
		HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		ComScope()
		{
			if (result != RPC_E_CHANGED_MODE)
				RequireSuccess(result, "Failed to initialize COM for texture loading.");
		}
		~ComScope() { if (SUCCEEDED(result)) CoUninitialize(); }
	};
}

void ShaderResourceView::LoadFromFile(const std::string& filePath)
{
	// DirectXTex caches its WIC factory; keep COM alive between texture loads.
	thread_local ComScope com;
	const auto path = std::filesystem::u8path(filePath);
	const auto extension = path.extension().wstring();
	const std::string error = "Failed to load diffuse texture: " + filePath;
	DirectX::ScratchImage image;
	HRESULT hr;
	if (_wcsicmp(extension.c_str(), L".dds") == 0)
		hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	else if (_wcsicmp(extension.c_str(), L".tga") == 0)
		hr = DirectX::LoadFromTGAFile(path.c_str(), DirectX::TGA_FLAGS_NONE, nullptr, image);
	else if (_wcsicmp(extension.c_str(), L".hdr") == 0)
		hr = DirectX::LoadFromHDRFile(path.c_str(), nullptr, image);
	else
		hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, image);
	RequireSuccess(hr, error.c_str());

	// The material shader samples a single Texture2D, not an array or cubemap.
	const auto& metadata = image.GetMetadata();
	if (metadata.dimension != DirectX::TEX_DIMENSION_TEXTURE2D ||
		metadata.arraySize != 1 || metadata.IsCubemap())
		throw std::runtime_error("Diffuse textures must be a single 2D image: " + filePath);

	ComPtr<ID3D11ShaderResourceView> view;
	RequireSuccess(DirectX::CreateShaderResourceView(Graphics::GetInstance()->GetDevice().Get(),
		image.GetImages(), image.GetImageCount(), metadata, view.GetAddressOf()), error.c_str());
	CreateSampler();
	_srView = std::move(view);
}

ShaderResourceView::ShaderResourceView()
{
}

ShaderResourceView::~ShaderResourceView()
{
}

void ShaderResourceView::CreateShaderResourceView(const UINT& width, const UINT& height, const uint8_t* pixels)
{
	if (!pixels || !width || !height || width > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION ||
		height > D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION)
		throw std::runtime_error("Invalid diffuse texture pixels or dimensions.");
	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA data = {};

	data.pSysMem = pixels;
	data.SysMemPitch = width * 4;
	ComPtr<ID3D11Texture2D> texture;

	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateTexture2D(&desc, &data, texture.GetAddressOf());
	RequireSuccess(hr, "Failed to create the diffuse texture.");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	ComPtr<ID3D11ShaderResourceView> view;
	hr = Graphics::GetInstance()->GetDevice()->CreateShaderResourceView(texture.Get(), &srvDesc, view.GetAddressOf());
	RequireSuccess(hr, "Failed to create the diffuse shader resource view.");
	CreateSampler();
	_srView = std::move(view);
}

void ShaderResourceView::CreateSampler()
{
	if (!_sampler)
	{
		D3D11_SAMPLER_DESC sampler = {};
		sampler.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampler.AddressU = sampler.AddressV = sampler.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampler.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampler.MaxLOD = D3D11_FLOAT32_MAX;
		RequireSuccess(Graphics::GetInstance()->GetDevice()->CreateSamplerState(&sampler,
			_sampler.GetAddressOf()), "Failed to create the diffuse sampler.");
	}
}

void ShaderResourceView::Render()
{
	Graphics::GetInstance()->GetContext()->PSSetShaderResources(0, 1, _srView.GetAddressOf());
	Graphics::GetInstance()->GetContext()->PSSetSamplers(0, 1, _sampler.GetAddressOf());
}
