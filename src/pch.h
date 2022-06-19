#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_REDUX_VERSION 1.3f

#include "cleo_redux_sdk.h"
#include "imgui.h"
#include "imgui_stdlib.h"
#include "scriptextender.hpp"

enum class eRenderer
{
    DX9,
    DX11,
    OPENGL,
    UNKNOWN
};

enum class eGameVer
{
    III,
    VC,
    SA,
    III_DE,
    VC_DE,
    SA_DE,
    RE_III,
    RE_VC,
    RE_SA,
    UNKNOWN
};

extern eRenderer gRenderer;
extern eGameVer gGameVer;
