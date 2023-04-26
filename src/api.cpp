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

#include "api.h"
#include "graphiccontext.h"
#include "capturemanager.h"
#include "compositor.h"
#include <dxgidebug.h>
#include <dxgi1_3.h>

Tako::GraphicContext* g_GraphicContext;
Tako::CaptureManager* g_CaptureManager;
Tako::Compositor* g_Compositor;

Tako::TakoError Tako::Initialize()
{
    TakoError err;

    g_GraphicContext = new Tako::GraphicContext();
    err = g_GraphicContext->Initialize();
    if (err != TakoError::OK)
        return err;

    g_CaptureManager = new Tako::CaptureManager();
    err = g_CaptureManager->Initialize();
    if (err != TakoError::OK)
        return err;

    g_Compositor = new Tako::Compositor();
    err = g_Compositor->Initialize();
    if (err != TakoError::OK)
        return err;

    return TakoError::OK;
}

Tako::TakoError Tako::Shutdown()
{
    TakoError err;

    err = g_CaptureManager->Shutdown();
    if (err != TakoError::OK)
        return err;
    delete g_CaptureManager;

    err = g_Compositor->Shutdown();
    if (err != TakoError::OK)
        return err;
    delete g_Compositor;

    err = g_GraphicContext->Shutdown();
    if (err != TakoError::OK)
        return err;
    delete g_GraphicContext;

    return TakoError::OK;
}

Tako::TakoError Tako::CaptureIntoBuffer(HANDLE bufferHandle, TakoRect targetRect)
{
    TakoError err;

    static TakoDisplayBuffer overlappedDisplays[MaxNumDisplays];
    uint32_t numDisplays;

    err = g_CaptureManager->Capture(targetRect, overlappedDisplays, &numDisplays);
    if (err != TakoError::OK)
        return err;

    g_Compositor->RenderComposite(bufferHandle, targetRect, overlappedDisplays, numDisplays);
    if (err != TakoError::OK)
        return err;

    return TakoError::OK;
}

