#include "../pch.h"
#include "BlendState.h"

BlendState::BlendState()
{
}

BlendState::~BlendState()
{
}

void BlendState::CreateBlendState(const BlendType& type)
{
	_type = type;
	D3D11_BLEND_DESC desc = {};
	desc.AlphaToCoverageEnable = false;
	desc.IndependentBlendEnable = false;
	D3D11_RENDER_TARGET_BLEND_DESC& renderTargetDesc = desc.RenderTarget[0];
	renderTargetDesc.SrcBlend = D3D11_BLEND_ONE;
	renderTargetDesc.DestBlend = D3D11_BLEND_ZERO;
	renderTargetDesc.BlendOp = D3D11_BLEND_OP_ADD;
	renderTargetDesc.SrcBlendAlpha = D3D11_BLEND_ONE;
	renderTargetDesc.DestBlendAlpha = D3D11_BLEND_ZERO;
	renderTargetDesc.BlendOpAlpha = D3D11_BLEND_OP_ADD;
	renderTargetDesc.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	switch (type)
	{
	case BlendType::BT_OPAQUE:
		renderTargetDesc.BlendEnable = false;
		break;

	case BlendType::BT_ALPHA:
		renderTargetDesc.BlendEnable = true;
		break;
	}
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreateBlendState(
		&desc,
		_state.ReleaseAndGetAddressOf()
	);
	CHECK(hr);
}

void BlendState::Render()
{
	Graphics::GetInstance()->GetContext()->OMSetBlendState(_state.Get(), nullptr, 0xFFFFFFFF);
}
