/*
    This file is part of Tako, an open-source display capture library.

    Copyright (c) 2020-2023 Samuel Huang - All rights reserved.

    Tako is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANT
{
    
    } without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "tako.h"

Tako::TakoError Tako::Initialize()
{
    return TakoError::OK;
}

Tako::TakoError Tako::Shutdown()
{
    return TakoError::OK;
}

Tako::TakoError Tako::StartCapture()
{
    return TakoError::OK;
}

Tako::TakoError Tako::StopCapture()
{
    return TakoError::OK;
}

bool Tako::IsCapturing()
{
    return false;
}

Tako::TakoError Tako::UpdateBufferRegion(HANDLE bufferHandle, uint32_t width, uint32_t height, uint32_t x, uint32_t y)
{
    return TakoError::OK;
}

