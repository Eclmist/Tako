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

#include "d3d11context.h"

Tako::TakoError Tako::D3D11Context::Initialize()
{
    TakoError err;

    err = InitializeD3D11();
    if (err != TakoError::OK)
        return err;

    return TakoError::OK;
}

Tako::TakoError Tako::D3D11Context::Shutdown()
{
    return TakoError::OK;
}

Tako::TakoError Tako::D3D11Context::InitializeD3D11()
{
    TakoError err;

    err = InitializeDevice();
    if (err != TakoError::OK) return err;

    err = InitializeDxgi();
    if (err != TakoError::OK) return err;

    return TakoError::OK;
}

Tako::TakoError Tako::D3D11Context::InitializeDevice()
{
    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };
    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_1
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);
    D3D_FEATURE_LEVEL currentFeatureLevel;

    HRESULT hr;

    // Create device
    for (UINT i = 0; i < numDriverTypes; ++i)
    {
        UINT createDeviceFlags = 0;
#ifdef _DEBUG 
        createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

        hr = D3D11CreateDevice(nullptr, driverTypes[i], nullptr, D3D11_CREATE_DEVICE_DEBUG, featureLevels, numFeatureLevels,
            D3D11_SDK_VERSION, &m_Device, &currentFeatureLevel, &m_DeviceContext);

        if (SUCCEEDED(hr))
            break;
    }

    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    return TakoError::OK;
}

Tako::TakoError Tako::D3D11Context::InitializeDxgi()
{
    HRESULT hr = m_Device->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(m_DxgiDevice.GetAddressOf()));
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    hr = m_DxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(m_DxgiAdapter.GetAddressOf()));
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    hr = m_DxgiAdapter->GetParent(__uuidof(IDXGIFactory2), reinterpret_cast<void**>(m_DxgiFactory.GetAddressOf()));
    if (FAILED(hr))
        return TakoError::DX11_ERROR;

    return TakoError::OK;
}



