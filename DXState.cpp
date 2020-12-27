#include "DXState.hpp"
#include "Vertex.hpp"

DXState::DXState(HWND hWindowHandle, HINSTANCE hInstance, UINT iWindowW, UINT iWindowH, std::string VertShader, std::string PixelShader, UINT uiTargetFPS) :
    m_hWindowHandle(hWindowHandle), 
    m_hInstance(hInstance),
    m_iWindowW(iWindowW),
    m_iWindowH(iWindowH),
    m_VertShaderFile(VertShader), 
    m_PixelShaderFile(PixelShader),
    m_uiTargetFPS(uiTargetFPS)
{
    m_pSquare = new DXVBO<5>();
    /*
    { -1.f, -1.f, 0.f },
    { 1.f, -1.f, 0.f },
    { 1.f, 1.f, 0.f },
    { -1.f, -1.f, 0.f },
    { -1.f, 1.f, 0.f },
    */

    m_pSquare->AddVertex(Vertex(  -1.f,  -1.f,   0.f ));
    m_pSquare->AddVertex(Vertex(   1.f,  -1.f,   0.f ));
    m_pSquare->AddVertex(Vertex(   1.f,   1.f,   0.f ));
    m_pSquare->AddVertex(Vertex(  -1.f,  -1.f,   0.f ));
    m_pSquare->AddVertex(Vertex(  -1.f,   1.f,   0.f ));
}

DXState::~DXState()
{
}

bool DXState::CreateDevice()
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount                          = 2;
    sd.BufferDesc.Width                     = 0;
    sd.BufferDesc.Height                    = 0;
    sd.BufferDesc.Format                    = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator     = 60;
    sd.BufferDesc.RefreshRate.Denominator   = 1;
    sd.Flags                                = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage                          = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow                         = m_hWindowHandle;
    sd.SampleDesc.Count                     = 1;
    sd.SampleDesc.Quality                   = 0;
    sd.Windowed                             = TRUE;
    sd.SwapEffect                           = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D10_CREATE_DEVICE_DEBUG;
    if (D3D10CreateDeviceAndSwapChain(NULL, D3D10_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, D3D10_SDK_VERSION, &sd, &this->m_pSwapChain, &this->m_pD3DDevice) != S_OK)
        return false;

    return true;
}

bool DXState::CreateRenderTarget()
{
    _DEVICE_CHECK(m_pD3DDevice);

    ID3D10Texture2D* pBackBuffer;
    m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

    if (!pBackBuffer)
    {
        ::MessageBoxA(0, "Fatal Error: pBackBuffer was zero!", "FATAL ERROR", 0);
        return false;
    }

    m_pD3DDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pMainRenderTargetView);

    if (!m_pMainRenderTargetView)
    {
        ::MessageBoxA(0, "Fatal Error: m_pMainRenderTargetView was zero!", "FATAL ERROR", 0);
        return false;
    }

    pBackBuffer->Release();

    return true;
}

bool DXState::CreateShaders()
{
    _DEVICE_CHECK(m_pD3DDevice);

    m_pVertexShader = new Shader<ShaderType_t::SHADER_TYPE_VERTEX>(m_pD3DDevice, "VertShader", "VertexShader.cso");
    m_pPixelShader = new Shader<ShaderType_t::SHADER_TYPE_PIXEL>(m_pD3DDevice, "PixelShader", "PixelShader.cso");

    return m_pVertexShader && m_pPixelShader;
}

bool DXState::CreateViewport()
{
    _DEVICE_CHECK(m_pD3DDevice);

    D3D10_VIEWPORT viewport = { 0 };

    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = this->m_iWindowW;
    viewport.Height = this->m_iWindowH;

    m_pD3DDevice->RSSetViewports(1, &viewport);

    return true;
}

bool DXState::CreateInputlayer()
{
    _DEVICE_CHECK(m_pD3DDevice);

    D3D10_INPUT_ELEMENT_DESC ied[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0},
    };

    m_pD3DDevice->CreateInputLayout(ied, ARRAYSIZE(ied), m_pVertexShader->GetFileData()->data(), m_pVertexShader->GetFileData()->size(), &m_pInputLayer);
    m_pD3DDevice->IASetInputLayout(m_pInputLayer.Get());

    return true;
}

bool DXState::CreateBlendState()
{
    _DEVICE_CHECK(m_pD3DDevice);

    D3D10_BLEND_DESC desc;
    ZeroMemory(&desc, sizeof(desc));
    desc.AlphaToCoverageEnable = false;
    desc.BlendEnable[0] = true;
    desc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
    desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
    desc.BlendOp = D3D10_BLEND_OP_ADD;
    desc.SrcBlendAlpha = D3D10_BLEND_INV_DEST_ALPHA; //
    desc.DestBlendAlpha = D3D10_BLEND_ONE; //
    desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;
    desc.RenderTargetWriteMask[0] = D3D10_COLOR_WRITE_ENABLE_ALL;

    m_pD3DDevice->CreateBlendState(&desc, &m_pBlendState);

    if (!m_pBlendState)
    {
        throw std::runtime_error{ "Failed to initialise blend state (DX)" };
    }

    m_pD3DDevice->OMSetBlendState(m_pBlendState, nullptr, 0xffffffff);

    return true;
}

bool DXState::CreateConstantBuffers()
{
    _DEVICE_CHECK(m_pD3DDevice);

    D3D10_BUFFER_DESC vb_bd = { 0 };
    vb_bd.ByteWidth = 16 * 2;
    vb_bd.Usage = D3D10_USAGE_DEFAULT;
    vb_bd.BindFlags = D3D10_BIND_CONSTANT_BUFFER;

    m_pD3DDevice->CreateBuffer(&vb_bd, nullptr, &m_pConstantBuffer);
    m_pD3DDevice->VSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());
    m_pD3DDevice->PSSetConstantBuffers(0, 1, m_pConstantBuffer.GetAddressOf());

    return true;
}


bool DXState::CreateVBOs()
{
    _DEVICE_CHECK(m_pD3DDevice);

    m_pSquare->CreateBuffer(m_pD3DDevice);

    return true;
}

bool DXState::DrawVBOs()
{
    // _DEVICE_CHECK(m_pD3DDevice);

    m_pSquare->DrawBuffer(m_pD3DDevice, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    return true;
}

bool DXState::ClearColour(float* pColour)
{
    // _DEVICE_CHECK(m_pD3DDevice);

    m_pD3DDevice->OMSetRenderTargets(1, &m_pMainRenderTargetView, NULL);
    m_pD3DDevice->ClearRenderTargetView(m_pMainRenderTargetView, pColour);

    return true; 
}

bool DXState::ClearColour(float r, float g, float b, float a)
{
    const float colour[4] = { r, g, b, a };
    m_pD3DDevice->OMSetRenderTargets(1, &m_pMainRenderTargetView, NULL);
    m_pD3DDevice->ClearRenderTargetView(m_pMainRenderTargetView, colour);

    return true;
}

bool DXState::Present()
{
    // _DEVICE_CHECK(m_pD3DDevice);
   
    this->m_pSwapChain->Present((UINT)m_pVSyncEnabled, 0);

    return true;
}

bool DXState::Cleanup()
{
    _DEVICE_CHECK(m_pD3DDevice);

    CleanupRenderTarget();

    if (m_pSwapChain) { m_pSwapChain->Release(); m_pSwapChain = NULL; }
    if (m_pD3DDevice) { m_pD3DDevice->Release(); m_pD3DDevice = NULL; }

    return true;
}

bool DXState::CleanupRenderTarget()
{
    if (m_pMainRenderTargetView)
    {
        m_pMainRenderTargetView->Release(); 
        m_pMainRenderTargetView = NULL; 
    }

    return true;
}

void DXState::SetNewScreenSize(UINT iWidth, UINT iHeight)
{
    m_iWindowW = iWidth;
    m_iWindowH = iHeight;
}