#pragma once

#ifndef WINDOWMANAGERV2_HPP
#define WINDOWMANAGERV2_HPP

#include <iostream>
#include <vector>
#include <Windows.h>

#include "imgui-master/imgui.h"
#include "imgui-master/backends/imgui_impl_dx10.h"
#include "imgui-master/backends/imgui_impl_win32.h"

#include "DXState.hpp"
#include "SimpleProfiler.hpp"

#define CPP_INCLUDE
#include "VariableBuffer.hpp"

#define CLAMP_FLOAT(n, mi, ma) ([](float f){ if (f > ma){return ma;} if (f < mi){return mi;}return f; })(n)
#define MAX_INTENSITY_VALUES_TRACKED 200
#define FLASH_TOGGLE_VIRT_KEY VK_SPACE

class WindowManager;

typedef void(*RenderFunction_t)(WindowManager* pInstance);

class WindowManager
{
private:

	RenderFunction_t m_pRenderFunc = nullptr;

	DXState* m_pDX = nullptr;
	VariableBuffer* m_pPassedVariables;
	SimpleProfiler* m_pProfiler;

	HWND m_hWindowHandle;
	HINSTANCE m_hInstance;

	std::wstring m_WindowTitle;
	const wchar_t* m_wszWindowClassname;
	unsigned long long m_iWindowW = 0, m_iWindowH = 0, m_iTargetFPS = 60;
	bool m_bInitialised = false, m_bQuit = false, m_bLockLight2Mouse = false;;

	std::vector<float> m_vecIntensityValues;
	unsigned int m_nIntensityValuesPosition = 0;

	std::vector<ImFont*> m_vecFonts;

public:

	bool m_bDrawDX = false;

	WindowManager(std::wstring Title, unsigned long long iWidth = 800, unsigned long long iHeight = 600, unsigned long long iTargetFPS = 60);
	~WindowManager(); // We should be able to call `delete <windowInstance>` to close the window.

	void Initialise(HINSTANCE hInstance, const wchar_t* wszWindowClassname);
	void InitialiseImGui_Dark();
	void Show();
	void ShowMenu();
	void UpdateVariables();
	void Destroy();

	void AddFont(ImFont* pFont);
	std::vector<ImFont*> Fonts();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

extern WindowManager* __g_pCurrentWindowInstance;

#endif