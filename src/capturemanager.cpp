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

#include "capturemanager.h"
#include "d3d11context.h"

extern Tako::D3D11Context* g_D3D11Context;

Tako::TakoError Tako::CaptureManager::Initialize()
{
    TakoError err;

    err = InitializeDxgiOutputs();
    if (err != TakoError::OK)
        return err;

    err = InitializeDesktopRect();
    if (err != TakoError::OK)
        return err;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::Shutdown()
{
    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::Capture(TakoRect targetRect, TakoDisplayBuffer* outDisplays, uint32_t* outNumBuffers)
{

    TakoError err = Capture(0, outDisplays);
    *outNumBuffers = 1;

    //for (wrl::ComPtr<IDXGIOutput1> display : m_DxgiOutputs)
    //{
    //    if (true /*targetRect.Intersect(display.getRect())*/)
    //    {
    //        // append to outBuffers
    //        //Capture()
    //    }
    //}

    return err;
}

Tako::TakoError Tako::CaptureManager::InitializeDxgiOutputs()
{
    // Enumerate the available adapters (i.e., graphics cards)
    IDXGIAdapter1* adapter = nullptr;
    for (uint32_t adapterIndex = 0; g_D3D11Context->GetDxgiFactory()->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
    {
        // Enumerate the available outputs (i.e., display connectors) for this adapter
        IDXGIOutput* output = nullptr;
        for (uint32_t outputIndex = 0; adapter->EnumOutputs(outputIndex, &output) != DXGI_ERROR_NOT_FOUND; outputIndex++)
        {
            // QI for Output 1
            IDXGIOutput1* dxgiOutput1 = nullptr;
            HRESULT hr = output->QueryInterface(__uuidof(dxgiOutput1), reinterpret_cast<void**>(&dxgiOutput1));
            if (FAILED(hr))
                continue;

            output->Release();
            output = nullptr;

            IDXGIOutputDuplication* duplicator;
            hr = dxgiOutput1->DuplicateOutput(g_D3D11Context->GetDevice().Get(), &duplicator);

            if (FAILED(hr))
                continue;

            m_DxgiDuplications.push_back(duplicator);
            m_DxgiOutputs.emplace_back(dxgiOutput1);

            ID3D11Texture2D* outputTexture;
            TakoError err = CreateOutputTexture(outputIndex, &outputTexture);
            if (err == TakoError::OK)
                m_CapturedTextures.emplace_back(outputTexture);
        }

        adapter->Release();
    }

    // If not outputs can be found, then the system must be in a transition
    // Should re-attempt again later.
    if (m_DxgiOutputs.size() <= 0)
        return TakoError::EXPECTED_ERROR;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::InitializeDesktopRect()
{
    // Set desktop bounds;
    LONG left = 0, right = 0, top = 0, bottom = 0;
    for (uint32_t i = 0; i < m_DxgiOutputs.size(); ++i)
    {
        DXGI_OUTPUT_DESC desc;
        HRESULT hr = m_DxgiOutputs[i]->GetDesc(&desc);

        if (FAILED(hr))
            return TakoError::UNEXPECTED_ERROR;

        left = std::min(left, desc.DesktopCoordinates.left);
        right = std::max(right, desc.DesktopCoordinates.right);
        top = std::min(top, desc.DesktopCoordinates.top);
        bottom = std::max(bottom, desc.DesktopCoordinates.bottom);

        assert(left < right);
        assert(top < bottom);
    }

    m_DesktopRect.m_Width = right - left;
    m_DesktopRect.m_Height = bottom - top;
    m_DesktopRect.m_X = left;
    m_DesktopRect.m_Y = right;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::Capture(uint32_t displayIndex, TakoDisplayBuffer* out)
{
    TakoError err;

    ID3D11Texture2D* srcTexture = nullptr;
    err = AcquireNextFrame(displayIndex, &srcTexture, &out->m_DisplayRect);
    if (err != TakoError::OK)
        return err;

    g_D3D11Context->GetDeviceContext()->CopyResource(m_CapturedTextures[displayIndex].Get(), srcTexture);

    err = ReleaseFrame(displayIndex, srcTexture);
    if (err != TakoError::OK)
        return err;

    out->m_Buffer = m_CapturedTextures[displayIndex];
    out->m_DisplayIndex = displayIndex;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::CreateOutputTexture(uint32_t displayIndex, ID3D11Texture2D** out)
{
    DXGI_OUTPUT_DESC displayDesc;
    m_DxgiOutputs[displayIndex]->GetDesc(&displayDesc);

    D3D11_TEXTURE2D_DESC desc;
    RtlZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
    desc.Width = displayDesc.DesktopCoordinates.right - displayDesc.DesktopCoordinates.left;
    desc.Height = displayDesc.DesktopCoordinates.bottom - displayDesc.DesktopCoordinates.top;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HRESULT hr = g_D3D11Context->GetDevice()->CreateTexture2D(&desc, nullptr, out);
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::AcquireNextFrame(int32_t displayIndex, ID3D11Texture2D** out, TakoRect* outRect)
{
    DXGI_OUTPUT_DESC displayDesc;
    m_DxgiOutputs[displayIndex]->GetDesc(&displayDesc);

    IDXGIResource* outResource = nullptr;

    while (true)
    {
        DXGI_OUTDUPL_FRAME_INFO frameInfo;
        HRESULT hr = m_DxgiDuplications[displayIndex]->AcquireNextFrame(INFINITE, &frameInfo, &outResource);

        if (hr == DXGI_ERROR_WAIT_TIMEOUT)
            continue;

        if (FAILED(hr))
            return TakoError::DX11_ERROR;

        break;
    }

    if ((*out) != nullptr)
        (*out)->Release();

    HRESULT hr = outResource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(out));
    outResource->Release();

    outRect->m_Width = displayDesc.DesktopCoordinates.right - displayDesc.DesktopCoordinates.left;
    outRect->m_Height = displayDesc.DesktopCoordinates.bottom - displayDesc.DesktopCoordinates.top;
    outRect->m_X = displayDesc.DesktopCoordinates.left;
    outRect->m_Y = displayDesc.DesktopCoordinates.right;

    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureManager::ReleaseFrame(int32_t displayIndex, ID3D11Texture2D* frame)
{
    HRESULT hr = m_DxgiDuplications[displayIndex]->ReleaseFrame();
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    return TakoError::OK;
}

