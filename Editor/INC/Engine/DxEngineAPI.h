#pragma once
#ifdef DX_ENGINE_EXPORTS
#define DX_ENGINE_API __declspec(dllexport)
#else
#define DX_ENGINE_API __declspec(dllimport)
#endif