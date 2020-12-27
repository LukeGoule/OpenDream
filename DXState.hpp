#pragma once

#ifndef DXSTATE_HPP
#define DXSTATE_HPP

#include <wrl/client.h>
#include <string>

#include "inc.hpp"
#include "Shader.hpp"
#include "DXVertexBufObject.hpp"

#define _DEVICE_CHECK(pDev) if (!pDev) { ::MessageBoxA(0, "Fatal Error: " #pDev " was zero!", "FATAL ERROR", 0); return false; }

class DXState
{
	Shader<ShaderType_t::SHADER_TYPE_VERTEX>* m_pVertexShader;
	Shader<ShaderType_t::SHADER_TYPE_PIXEL>* m_pPixelShader;

	Microsoft::WRL::ComPtr<ID3D10InputLayout> m_pInputLayer;
	Microsoft::WRL::ComPtr<ID3D10Buffer> m_pVBO;
	Microsoft::WRL::ComPtr<ID3D10Buffer> m_pConstantBuffer;

	ID3D10BlendState* m_pBlendState = NULL;

	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;
	UINT m_uiTargetFPS;

	std::string m_VertShaderFile, m_PixelShaderFile;

	UINT m_iWindowW, m_iWindowH;

	DXVBO<5>* m_pSquare = nullptr;

public:

	bool m_pVSyncEnabled = true;

	ID3D10Device* m_pD3DDevice = NULL;
	IDXGISwapChain* m_pSwapChain = NULL;
	ID3D10RenderTargetView* m_pMainRenderTargetView = NULL;

	DXState(HWND hWindowHandle, HINSTANCE hInstance, UINT iWindowW, UINT iWindowH, std::string VertShader, std::string PixelShader, UINT uiTargetFPS = 60);
	~DXState();

	bool CreateDevice();
	bool CreateRenderTarget();
	bool CreateShaders();
	bool CreateViewport();
	bool CreateInputlayer();
	bool CreateBlendState();
	bool CreateConstantBuffers();

	bool CreateVBOs();
	bool DrawVBOs();

	bool ClearColour(float* pColour);
	bool ClearColour(float r, float g, float b, float a);
	bool Present();

	bool Cleanup();
	bool CleanupRenderTarget();

	template<typename T> bool UpdateConstantBuffer(T* pUpdatedValues)
	{
		assert(sizeof(T) % 16 == 0);

		m_pD3DDevice->UpdateSubresource(m_pConstantBuffer.Get(), 0, 0, pUpdatedValues, 0, 0);

		return true;
	}

	void SetNewScreenSize(UINT iWidth, UINT iHeight);
};

#endif