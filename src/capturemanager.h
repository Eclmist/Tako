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
    class CaptureManager
    {
    public:
        CaptureManager() = default;
        ~CaptureManager() = default;

        TakoError Initialize();
        TakoError Shutdown();
        TakoError Capture(TakoRect targetRect, TakoDisplayBuffer* outDisplays, uint32_t* outNumBuffers);

    private:
        TakoError InitializeDxgiOutputs();
        TakoError InitializeDesktopRect();
        TakoError Capture(uint32_t displayIndex, TakoDisplayBuffer* out);
        TakoError CreateOutputTexture(uint32_t displayIndex, ID3D11Texture2D** out);
        TakoError AcquireNextFrame(int32_t displayIndex, ID3D11Texture2D** out, DXGI_OUTDUPL_FRAME_INFO* outFrame);
        TakoError ReleaseFrame(int32_t displayIndex, ID3D11Texture2D* frame);

    private:
        std::vector<wrl::ComPtr<IDXGIOutput1>> m_DxgiOutputs;
        std::vector<wrl::ComPtr<IDXGIOutputDuplication>> m_DxgiDuplications;
        std::vector<wrl::ComPtr<ID3D11Texture2D>> m_CapturedTextures;

        TakoRect m_DesktopRect; // A rect that represents the entire desktop comprised of all displays
    };
}

