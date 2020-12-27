#include "Window.hpp"
#include "Shader.hpp"
#include "ImGui_Ext.hpp"
#include "resource.h"

#include <vector>
#include <string>
#include <assert.h>
#include <chrono>
#include <wrl/client.h>

WindowManager* __g_pCurrentWindowInstance = nullptr;

WindowManager::WindowManager(std::wstring Title, unsigned long long iWidth, unsigned long long iHeight, unsigned long long iTargetFPS)
    : m_WindowTitle(Title), 
    m_iWindowW(iWidth), 
    m_iWindowH(iHeight), 
    m_iTargetFPS(iTargetFPS)
{
    __g_pCurrentWindowInstance = this;

    m_pPassedVariables = new VariableBuffer;
    m_pProfiler = new SimpleProfiler();

    m_vecIntensityValues = std::vector<float>(MAX_INTENSITY_VALUES_TRACKED);
}

WindowManager::~WindowManager()
{
    this->Destroy();
}


void WindowManager::Initialise(HINSTANCE hInstance, const wchar_t* wszWindowClassname)
{
    assert(!m_bInitialised);

    this->m_hInstance = hInstance;
    this->m_wszWindowClassname = wszWindowClassname;

    WNDCLASSEX wc = {
        sizeof(WNDCLASSEX),
        CS_CLASSDC,
        this->WndProc,
        0L,
        0L,
        GetModuleHandle(NULL),
        NULL,
        NULL,
        NULL,
        NULL,
        this->m_wszWindowClassname,
        NULL
    };

    RegisterClassEx(&wc);

    this->m_hWindowHandle = CreateWindow(wc.lpszClassName, this->m_wszWindowClassname, 0, 100, 100, this->m_iWindowW, this->m_iWindowH, NULL, NULL, wc.hInstance, NULL);

    if (!this->m_hWindowHandle)
    {
        throw std::runtime_error{ "FATAL! Failed to initialise window!" };
    }

    SetWindowText(this->m_hWindowHandle, this->m_WindowTitle.c_str());

    SetWindowLongPtrA(m_hWindowHandle, GWL_STYLE, WS_OVERLAPPEDWINDOW);
    
    SetClassLong(m_hWindowHandle, GCLP_HICON, (LONG)LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1))); // set custom icon

    m_pDX = new DXState(
        m_hWindowHandle, 
        hInstance, 
        m_iWindowW, m_iWindowH, 
        "VertexShader.cso", "PixelShader.cso", m_iTargetFPS);

    if (!m_pDX->CreateDevice())
    {
        MessageBoxA(0, "FATAL! Failed to initialise DirectX", "FATAL ERROR", 0);
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        exit(-1);
    }

    m_pDX->CleanupRenderTarget();
    m_pDX->CreateShaders();
    m_pDX->CreateViewport();
    m_pDX->CreateInputlayer();
    m_pDX->CreateBlendState();
    m_pDX->CreateConstantBuffers();
    m_pDX->CreateVBOs();

    ShowWindow(this->m_hWindowHandle, SW_SHOWDEFAULT);
    UpdateWindow(this->m_hWindowHandle);

    this->m_bInitialised = true;
    __g_pCurrentWindowInstance = this;
}

void WindowManager::InitialiseImGui_Dark()
{
    // Sets up a neat looking dark mode for the ImGui instance.

    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGuiStyle* style = &ImGui::GetStyle();

    ImGui::StyleColorsDark();
    style->WindowPadding = ImVec2(5, 5);
    style->WindowRounding = 0.0f;
    style->FramePadding = ImVec2(5, 5);
    style->FrameRounding = 0.0f;
    style->ItemSpacing = ImVec2(5, 5);
    style->ItemInnerSpacing = ImVec2(10, 10);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 5.0f;
    style->ScrollbarRounding = 0.0f;
    style->GrabMinSize = 1.0f;
    style->GrabRounding = 0.0f;

    auto C = ImVec4(25.f / 255.f, 25.f / 255.f, 25.f / 255.f, 255.f);
    auto C2 = ImVec4(40.f / 255.f, 40.f / 255.f, 40.f / 255.f, 255.f);

    style->Colors[ImGuiCol_Border] = C;
    style->Colors[ImGuiCol_BorderShadow] = C;
    style->Colors[ImGuiCol_WindowBg] = C;
    style->Colors[ImGuiCol_FrameBgActive] = C;
    style->Colors[ImGuiCol_FrameBgHovered] = C;
    style->Colors[ImGuiCol_FrameBg] = C;

    ImGui::GetIO().IniFilename = nullptr;

    //this->m_pFontConsolas = io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 13.f);
   
    AddFont(io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 14.f)); // fonts[0]
    AddFont(io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\consola.ttf", 25.f)); // fonts[1]
    AddFont(io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdana.ttf", 25.f)); // fonts[2]
    AddFont(io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\verdanai.ttf", 12.f)); // fonts[3] -- verdana ital

    // Setup Platform/Renderer bindings
    ImGui_ImplWin32_Init(m_hWindowHandle);
    ImGui_ImplDX10_Init(m_pDX->m_pD3DDevice);
}

void WindowManager::Show()
{
    m_pProfiler->StartApp(); // Begins the accurate timer.

    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT && !this->m_bQuit)
    {
        m_pProfiler->FrameStart();

        __g_pCurrentWindowInstance = this;

        if (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            continue;
        }

        m_pDX->ClearColour(0.05f, 0.05f, 0.05f, 1.0f);
        
        if (this->m_bDrawDX)
        {
            m_pDX->UpdateConstantBuffer<VariableBuffer>(m_pPassedVariables);

            m_pDX->DrawVBOs();
        }
        
        ImGui_ImplDX10_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        ShowMenu();

        // Rendering
        ImGui::Render();
        ImGui_ImplDX10_RenderDrawData(ImGui::GetDrawData());

        m_pDX->Present();

        m_pProfiler->FrameEnd();

        UpdateVariables();
    }
}

void WindowManager::ShowMenu()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowBgAlpha(.75f);
    ImGui::SetNextWindowSize(ImVec2(m_iWindowW / 4, m_iWindowH));
    ImGui::Begin("OpenDream Configuration Menu", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    {
        ImGui::PushFont(Fonts()[2]);
        {
            ImGui::TextColored(ImVec4(0.9f, 0.9f, 0.9f, 1.f), "OpenDream");
        }
        ImGui::PopFont();

        ImGui::PushFont(Fonts()[3]);
        {
            ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.f), "    \"trip without drugs\"");
        }
        ImGui::PopFont();

        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        ImGui::PushItemWidth((m_iWindowW / 4) - (ImGui::CalcTextSize("Force Intensity").x * 1.25f));
        {
            ImGui::PushFont(Fonts()[0]);
            {
                ImGui::QuickLabel("Flash Settings");
                {
                    ImGui::SliderFloat("Flick Rate", &m_pPassedVariables->flick_rate, 0.f, 20.f);
                    ImGui::SliderFloat("Force Intensity", &m_pPassedVariables->forced_intensity, .01f, 10.f);
                }

                ImGui::QuickLabel("Colour");
                {
                    ImGui::SliderFloat("Red Output", &m_pPassedVariables->red_mod, 0.f, 1.f);
                    ImGui::SliderFloat("Green Output", &m_pPassedVariables->green_mod, 0.f, 1.f);
                    ImGui::SliderFloat("Blue Output", &m_pPassedVariables->blue_mod, 0.f, 1.f);
                }

                ImGui::Spacing();

                ImGui::QuickLabel("Intensity Output :: I = sin(time * flick_rate * 10)");
                {
                    // Intensity value graph
                    ImGui::PlotLines("", (const float*)m_vecIntensityValues.data(), MAX_INTENSITY_VALUES_TRACKED, 0, 0, -1.f, 1.f, ImVec2((m_iWindowW / 4) - 50.f, 75.f), 4);
                }

                ImGui::Spacing();

                ImGui::QuickLabel("Misc");
                {
                    ImGui::Checkbox("Show (Space to toggle)", &m_bDrawDX);
                    ImGui::Checkbox("Quit", &m_bQuit);
                }

                ImGui::Spacing();

                ImGui::QuickLabel("Performance");
                {
                    ImGui::Text("FPS: %d (min: %d, max: %d, time: %.3f)\n", m_pProfiler->FPSAverage(), m_pProfiler->FPSMin(), m_pProfiler->FPSMax(), m_pProfiler->SecondsSinceStart());

                    // FPS Graph
                    ImGui::PlotLines("", (const float*)m_pProfiler->History(), MAX_FRAME_HISTORY, 0, 0, 0.f, 200.f, ImVec2((m_iWindowW / 4) - 50.f, 75.f), 4);
                }
            }
            ImGui::PopItemWidth();
        }
        ImGui::PopFont();
    }
    ImGui::End();
}

void WindowManager::UpdateVariables()
{
    m_pPassedVariables->time = m_pProfiler->SecondsSinceStart();

    POINT p;
    if (m_bLockLight2Mouse && GetCursorPos(&p))
    {
        if (ScreenToClient(m_hWindowHandle, &p))
        {
            m_pPassedVariables->mousex = (((float)p.x) - (m_iWindowW / 2)) / (m_iWindowW / 2);
            m_pPassedVariables->mousey = ((m_iWindowH / 2) - ((float)p.y)) / (m_iWindowH / 2);
        }
    }

    if (!m_bLockLight2Mouse)
    {
        m_pPassedVariables->mousex = 0.f;
        m_pPassedVariables->mousey = 0.f;
    }

    m_vecIntensityValues[m_nIntensityValuesPosition] = sin(m_pPassedVariables->time * m_pPassedVariables->flick_rate * 10) * CLAMP_FLOAT(m_pPassedVariables->forced_intensity, 0.f, 1.f);

    if (m_nIntensityValuesPosition == MAX_INTENSITY_VALUES_TRACKED - 1)
    {
        m_nIntensityValuesPosition = 0;
    }

    m_nIntensityValuesPosition++;

    static bool pressed = false;

    if (GetAsyncKeyState(FLASH_TOGGLE_VIRT_KEY) && !pressed)
    {
        m_bDrawDX = !m_bDrawDX;

        pressed = true;
    }

    if (pressed)
    {
        if (!GetAsyncKeyState(FLASH_TOGGLE_VIRT_KEY))
        {
            pressed = false;
        }
    }
}

void WindowManager::Destroy()
{
    assert(m_bInitialised);

    ImGui_ImplDX10_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    m_pDX->Cleanup();

    DestroyWindow(this->m_hWindowHandle);
}

void WindowManager::AddFont(ImFont* pFont)
{
    this->m_vecFonts.push_back(pFont);
}

std::vector<ImFont*> WindowManager::Fonts()
{
    return this->m_vecFonts;
}


// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*static*/ LRESULT WINAPI WindowManager::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (__g_pCurrentWindowInstance->m_pDX->m_pD3DDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            // Update our window instance.
            __g_pCurrentWindowInstance->m_iWindowW = (UINT)LOWORD(lParam);
            __g_pCurrentWindowInstance->m_iWindowH = (UINT)HIWORD(lParam);

            // Update the DX instance;
            __g_pCurrentWindowInstance->m_pDX->CleanupRenderTarget();
            __g_pCurrentWindowInstance->m_pDX->m_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            __g_pCurrentWindowInstance->m_pDX->SetNewScreenSize((UINT)LOWORD(lParam), (UINT)HIWORD(lParam));
            __g_pCurrentWindowInstance->m_pDX->CreateViewport();
            __g_pCurrentWindowInstance->m_pDX->CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}