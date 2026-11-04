#include "../pch.h"
#include "Shaders.h"

void PixelShader::CreateShader(const wchar_t* hlslPath)
{
	Graphics::GetInstance()->CompileFromFile(hlslPath, "PS", "ps_5_0", blob);
	HRESULT hr = Graphics::GetInstance()->GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, _shader.GetAddressOf());
	CHECK(hr);
}

void PixelShader::Render()
{
	Graphics::GetInstance()->GetContext()->PSSetShader(_shader.Get(), nullptr, 0);
}