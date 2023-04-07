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
    class Compositor
    {
    public:
        Compositor() = default;
        ~Compositor() = default;

        TakoError Initialize();
        TakoError Shutdown();

    public:
        TakoError RenderComposite(HANDLE outTexture, TakoRect targetRect, TakoDisplayBuffer* displays, uint32_t numDisplays = 1);

    private:
        TakoError InitializeShaders();

    private:
        wrl::ComPtr<ID3D11RenderTargetView> m_TargetTextureRtv;
        wrl::ComPtr<ID3D11SamplerState> m_Sampler;
        wrl::ComPtr<ID3D11BlendState> m_BlendState;
        wrl::ComPtr<ID3D11VertexShader> m_VertexShader;
        wrl::ComPtr<ID3D11PixelShader> m_PixelShader;
        wrl::ComPtr<ID3D11InputLayout> m_InputLayout;
    };
}

