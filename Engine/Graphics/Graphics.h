#pragma once

class Graphics
{
public:
	Graphics();
	~Graphics();
	static Graphics* GetInstance()
	{
		if (_instance == nullptr) {
			_instance = new Graphics();
		}
		return _instance;
	};
	void Init(const HWND& handle, const UINT& width, const UINT& height);
	void ClearViewports();
	void Render();

private:
	void BeginFrame();
	void EndFrame();
	void CreateDeviceAndSwapChain(const HWND& handle, const UINT& width, const UINT& height);
	void CreateDSV(const UINT& width, const UINT& height);
	void CreateRTV();
	void CreateViewport(const UINT& width, const UINT& height);
private:
	static Graphics* _instance;
	ComPtr<ID3D11Device> _device = nullptr;
	ComPtr<ID3D11DeviceContext> _context;
	ComPtr<IDXGISwapChain> _swapChain;
	ComPtr<ID3D11RenderTargetView> _rtView;
	ComPtr<ID3D11Texture2D> _dsBuffer;
	ComPtr<ID3D11DepthStencilView> _dsView;

};

