#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_REDUX_VERSION 3.0f
#define IMGUI_REDUX_VERSION_NUM 30000
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
    IV,
    V,
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
