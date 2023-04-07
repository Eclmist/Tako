/*
    This file is part of Tako, an open-source display capture library.

    Copyright (c) 2020-2023 Samuel Huang - All rights reserved.

    Tako is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANT without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "compositor.h"
#include "d3d11context.h"
#include "data/compositor_vs.h"
#include "data/compositor_ps.h"

extern Tako::D3D11Context g_D3D11Context;

Tako::TakoError Tako::Compositor::Initialize()
{
    TakoError err;
    
    err = InitializeShaders();
    if (err != TakoError::OK)
        return err;
    
    return TakoError::OK;
}

Tako::TakoError Tako::Compositor::Shutdown()
{
    return TakoError::OK;
}

Tako::TakoError Tako::Compositor::RenderComposite(HANDLE outTexture, TakoRect targetRect, TakoDisplayBuffer* displays, uint32_t numDisplays)
{
    // Query the ID3D11Texture2D interface from the shared resource.
    ID3D11Texture2D* texture = nullptr;
    IDXGIKeyedMutex* keyMutex = nullptr;

    HRESULT hr = g_D3D11Context.GetDevice()->OpenSharedResource(outTexture, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&texture));
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    hr = texture->QueryInterface(__uuidof(IDXGIKeyedMutex), reinterpret_cast<void**>(&keyMutex));
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    while (true)
    {
        HRESULT hr = keyMutex->AcquireSync(0, 1000);
        if (hr == static_cast<HRESULT>(WAIT_TIMEOUT))
            continue;

        if (FAILED(hr))
            return TakoError::DX11_ERROR;

        break;
    }

    // Set viewport
    D3D11_VIEWPORT vp;
    vp.Width = static_cast<FLOAT>(targetRect.m_Width);
    vp.Height = static_cast<FLOAT>(targetRect.m_Height);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    g_D3D11Context.GetDeviceContext()->RSSetViewports(1, &vp);

    // Vertices for drawing whole texture
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 TexCoord;

    struct Vertex {
        DirectX::XMFLOAT3 Pos;
        DirectX::XMFLOAT2 TexCoord;
    };

    static constexpr uint32_t NumVertices = 6;
    Vertex vertices[NumVertices] =
    {
        { DirectX::XMFLOAT3(-1.0f, -1.0f, 0), DirectX::XMFLOAT2(0.0f, 1.0f) },
        { DirectX::XMFLOAT3(-1.0f, 1.0f, 0), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 0), DirectX::XMFLOAT2(1.0f, 1.0f) },
        { DirectX::XMFLOAT3(1.0f, -1.0f, 0), DirectX::XMFLOAT2(1.0f, 1.0f) },
        { DirectX::XMFLOAT3(-1.0f, 1.0f, 0), DirectX::XMFLOAT2(0.0f, 0.0f) },
        { DirectX::XMFLOAT3(1.0f, 1.0f, 0), DirectX::XMFLOAT2(1.0f, 0.0f) },
    };

    D3D11_TEXTURE2D_DESC sharedTextureDesc;
    texture->GetDesc(&sharedTextureDesc);

    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = sharedTextureDesc.Format;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;

    //D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    //srvDesc.Format = sharedTextureDesc.Format;
    //srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    //srvDesc.Texture2D.MostDetailedMip = sharedTextureDesc.MipLevels - 1;
    //srvDesc.Texture2D.MipLevels = sharedTextureDesc.MipLevels;

    // Create new shader resource view
    ID3D11RenderTargetView* rtvResource = nullptr;
    hr = g_D3D11Context.GetDevice()->CreateRenderTargetView(texture, &rtvDesc, &rtvResource);
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    FLOAT blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
    g_D3D11Context.GetDeviceContext()->OMSetBlendState(nullptr, blendFactor, 0xffffffff);
    g_D3D11Context.GetDeviceContext()->OMSetRenderTargets(1, &rtvResource, nullptr);
    g_D3D11Context.GetDeviceContext()->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    g_D3D11Context.GetDeviceContext()->PSSetShader(m_PixelShader.Get(), nullptr, 0);
    //m_DeviceContext->PSSetShaderResources(0, 1, &rtvResource);
    //m_DeviceContext->PSSetSamplers(0, 1, m_Sampler.GetAddressOf());
    g_D3D11Context.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    D3D11_BUFFER_DESC bufferDesc;
    RtlZeroMemory(&bufferDesc, sizeof(bufferDesc));
    bufferDesc.Usage = D3D11_USAGE_DEFAULT;
    bufferDesc.ByteWidth = sizeof(Vertex) * NumVertices;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA initData;
    RtlZeroMemory(&initData, sizeof(initData));
    initData.pSysMem = vertices;

    // Create vertex buffer
    ID3D11Buffer* vertexBuffer = nullptr;
    hr = g_D3D11Context.GetDevice()->CreateBuffer(&bufferDesc, &initData, &vertexBuffer);
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    g_D3D11Context.GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

    // Draw textured quad onto render target
    g_D3D11Context.GetDeviceContext()->Draw(NumVertices, 0);

    // Release keyed mutex
    hr = keyMutex->ReleaseSync(0);
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    rtvResource->Release();
    vertexBuffer->Release();
    texture->Release();
    keyMutex->Release();

    return TakoError::OK;
}

Tako::TakoError Tako::Compositor::InitializeShaders()
{
    HRESULT hr;

    UINT size = ARRAYSIZE(g_VS_Main);
    hr = g_D3D11Context.GetDevice()->CreateVertexShader(g_VS_Main, size, nullptr, &m_VertexShader);
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    D3D11_INPUT_ELEMENT_DESC inputLayout[] =
    {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
    };

    UINT numElements = ARRAYSIZE(inputLayout);
    hr = g_D3D11Context.GetDevice()->CreateInputLayout(inputLayout, numElements, g_VS_Main, size, m_InputLayout.GetAddressOf());
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    g_D3D11Context.GetDeviceContext()->IASetInputLayout(m_InputLayout.Get());

    size = ARRAYSIZE(g_PS_Main);
    hr = g_D3D11Context.GetDevice()->CreatePixelShader(g_PS_Main, size, nullptr, &m_PixelShader);
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    return TakoError::OK;
}
