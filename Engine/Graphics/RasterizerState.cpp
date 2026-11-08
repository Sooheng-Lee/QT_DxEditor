#include "../pch.h"
#include "RasterizerState.h"

RasterizerState::RasterizerState()
{
}

RasterizerState::~RasterizerState()
{
}

void RasterizerState::CreateState(const RasterizerType& type)
{
	D3D11_RASTERIZER_DESC desc = {};
	switch (type)
	{
	case RT_SOLID:
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		break;
	case RT_WIREFRAME:
		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_NONE;
		break;
	}
	desc.FrontCounterClockwise = false;

	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;

	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;

	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateRasterizerState(&desc, _state.GetAddressOf());
	CHECK(hr);
}

void RasterizerState::Render()
{
	Graphics::GetInstance()->GetContext()->RSSetState(_state.Get());
}