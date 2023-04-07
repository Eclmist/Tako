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

#pragma once

#include "common.h"

namespace Tako
{
    class D3D11Context
    {
    public:
        D3D11Context() = default;
        ~D3D11Context() = default;

        TakoError Initialize();
        TakoError Shutdown();
        
    public:
        inline wrl::ComPtr<IDXGIAdapter> GetDxgiAdapter() const { return m_DxgiAdapter; }
        inline wrl::ComPtr<IDXGIFactory2> GetDxgiFactory() const { return m_DxgiFactory; }
        inline wrl::ComPtr<IDXGIDevice> GetDxgiDevice() const { return m_DxgiDevice; }
        inline wrl::ComPtr<ID3D11Device> GetDevice() const { return m_Device; }
        inline wrl::ComPtr<ID3D11DeviceContext> GetDeviceContext() const { return m_DeviceContext; }

    private:
        TakoError InitializeD3D11();
        TakoError InitializeDevice();
        TakoError InitializeDxgi();

    private:
        wrl::ComPtr<IDXGIAdapter> m_DxgiAdapter;
        wrl::ComPtr<IDXGIFactory2> m_DxgiFactory;
        wrl::ComPtr<IDXGIDevice> m_DxgiDevice;

        wrl::ComPtr<ID3D11Device> m_Device;
        wrl::ComPtr<ID3D11DeviceContext> m_DeviceContext;
    };
}

