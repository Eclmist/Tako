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

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <d3d11.h>
#include <dxgi1_2.h>
#include <DirectXMath.h>
#include <vector>
#include <cstdint>

#include <wrl.h>
namespace wrl = Microsoft::WRL;

#ifdef TACO_EXPORT_DLL
#define TAKO_API __declspec(dllexport)
#else
#define TAKO_API __declspec(dllimport)
#endif

static constexpr uint32_t MaxNumDisplays = 8;

namespace Tako
{
    struct TakoRect
    {
        int32_t m_X;
        int32_t m_Y;
        uint32_t m_Width;
        uint32_t m_Height;

        bool operator==(const TakoRect& other)
        {
            return other.m_X == m_X && other.m_Y == m_Y &&
                other.m_Width == m_Width && other.m_Height == m_Height;
        }
    };

    struct TakoDisplayBuffer
    {
        wrl::ComPtr<ID3D11Texture2D> m_Buffer;
        TakoRect m_DisplayRect;
        uint32_t m_DisplayIndex;
    };

    enum class TakoError : uint32_t
    {
        OK = 0,
        NOT_SUPPORTED = 1,
        DX11_ERROR = 2,
        EXPECTED_ERROR = 3,
        UNEXPECTED_ERROR = 4,
    };
}

