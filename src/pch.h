#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_REDUX_VERSION 1.5f

#include "imgui.h"
#include "imgui_stdlib.h"
#include "scriptextender.hpp"

enum class eRenderer {
    Dx9,
    Dx11,
    Dx12,
    OpenGL,
    Unknown
};

enum class eGameVer {
    III,
    VC,
    SA,
    III_DE,
    VC_DE,
    SA_DE,
    RE_III,
    RE_VC,
    RE_SA,
    BullySE,
    Unknown
};

extern eRenderer gRenderer;
extern eGameVer gGameVer;
extern void* gD3DDevice;
