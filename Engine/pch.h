#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>

#include <vector>
#include <assert.h>
#include <wrl/client.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Microsoft::WRL;
#define CHECK(hr) assert(SUCCEEDED(hr))
#include "VertexTypes.h"
#include "Graphics/BaseShapes.h"
#include "Graphics/Graphics.h"
#include "Graphics/Shaders.h"