#include "../pch.h"
#include "Graphics.h"

Graphics* Graphics::_instance = nullptr;

Graphics::Graphics()
{
}

Graphics::~Graphics()
{
	if (_instance)
	{
		delete _instance;
		_instance = nullptr;
	}
}

void Graphics::Init(const HWND& handle, const UINT& width, const UINT& height)
{
	CreateDeviceAndSwapChain(handle, width, height);
	CreateDSV(width, height);
	CreateRTV();
	CreateViewport(width, height);

}

void Graphics::ClearViewports()
{
	FLOAT ClearView[] = { 0.15f, 0.15f, 0.15f, 1.0f };
	_context->ClearRenderTargetView(_rtView.Get(), ClearView);
	_context->ClearDepthStencilView(_dsView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void Graphics::Render()
{
	BeginFrame();
	EndFrame();
}

void Graphics::CompileFromFile(const wchar_t* hlslPath, const char* entryPoint, const char* version, ComPtr<ID3DBlob> blob)
{
	ComPtr<ID3DBlob> errBlob = nullptr;
	UINT flag = 0;
#ifdef _DEBUG
	flag = D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_DEBUG;
#endif
	HRESULT hr = D3DCompileFromFile(hlslPath, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, version, flag, D3DCOMPILE_IEEE_STRICTNESS, blob.ReleaseAndGetAddressOf(), errBlob.GetAddressOf());
	if (errBlob)
	{
		OutputDebugStringA(static_cast<const char*>(errBlob->GetBufferPointer()));
	}
	CHECK(SUCCEEDED(hr));
}

void Graphics::BeginFrame()
{
	ClearViewports();
	_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void Graphics::EndFrame()
{
	_swapChain->Present(0, 0);
}

void Graphics::CreateDeviceAndSwapChain(const HWND& handle, const UINT& width, const UINT& height)
{
	DXGI_SWAP_CHAIN_DESC scDesc = {};
	{
		scDesc.BufferCount = 1;
		scDesc.BufferDesc.Width = width;
		scDesc.BufferDesc.Height = height;
		scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scDesc.BufferDesc.RefreshRate.Numerator = 60;
		scDesc.BufferDesc.RefreshRate.Denominator = 1;
		scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		scDesc.SampleDesc.Count = 1;
		scDesc.SampleDesc.Quality = 0;
		scDesc.Windowed = true;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.OutputWindow = handle;
	}
	const D3D_FEATURE_LEVEL FeatureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, FeatureLevels, ARRAYSIZE(FeatureLevels), D3D11_SDK_VERSION, &scDesc, _swapChain.GetAddressOf(), _device.GetAddressOf(), nullptr, _context.GetAddressOf());
	CHECK(hr);
}

void Graphics::CreateDSV(const UINT& width, const UINT& height)
{
	D3D11_TEXTURE2D_DESC bufferDesc = {};
	bufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	bufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	bufferDesc.ArraySize = 1;
	bufferDesc.MipLevels = 1;
	bufferDesc.Width = width;
	bufferDesc.Height = height;
	bufferDesc.SampleDesc.Count = 1;
	bufferDesc.SampleDesc.Quality = 0;
	HRESULT hr = _device->CreateTexture2D(&bufferDesc, nullptr, _dsBuffer.GetAddressOf());
	CHECK(hr);
	hr = _device->CreateDepthStencilView(_dsBuffer.Get(), nullptr, _dsView.GetAddressOf());
	CHECK(hr);
}

void Graphics::CreateRTV()
{
	ComPtr<ID3D11Texture2D> buffer;
	HRESULT hr = _swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(buffer.GetAddressOf()));
	CHECK(hr);
	hr = _device->CreateRenderTargetView(buffer.Get(), nullptr, _rtView.GetAddressOf());
	_context->OMSetRenderTargets(1, _rtView.GetAddressOf(), _dsView.Get());
	CHECK(hr);
}

void Graphics::CreateViewport(const UINT& width, const UINT& height)
{
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.MaxDepth = 1.0f;
	vp.MinDepth = 0.0f;
	vp.Width = static_cast<float>(width);
	vp.Height = static_cast<float>(height);
	_context->RSSetViewports(1, &vp);
}
