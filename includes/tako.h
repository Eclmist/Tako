/*
    This file is part of Tako, an open-source display capture library.

    Copyright (c) 2020-2023 Samuel Huang - All rights reserved.

    Tako is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <cstdint>
#include <d3d11.h>

#ifdef TACO_EXPORT_DLL
#define TAKO_API __declspec(dllexport)
#else
#define TAKO_API __declspec(dllimport)
#endif

namespace Tako {
    struct TakoRect
    {
        uint32_t m_X;
        uint32_t m_Y;
        uint32_t m_Width;
        uint32_t m_Height;
    };

    struct TakoDisplayBuffer
    {
        ID3D11Texture2D* m_Buffer;
        TakoRect m_Rect;
        uint32_t m_DisplayIndex;
    };

    enum class TakoError : uint32_t
    {
        OK = 0,
        FAILED = 0x7FFFFFFF,
        NOTSUPPORTED = 1,
    };

    TAKO_API TakoError Initialize();
    TAKO_API TakoError Shutdown();

    TAKO_API TakoError StartCapture();
    TAKO_API TakoError StopCapture();
    TAKO_API bool IsCapturing();

    TAKO_API TakoError UpdateBufferRegion(HANDLE bufferHandle, uint32_t width, uint32_t height, uint32_t x = 0, uint32_t y = 0);
}
