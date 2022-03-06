#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_REDUX_VERSION 0.1f

#include "cleo/cleo_redux_sdk.h"
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "scriptextender.hpp"

enum class eRenderer
{
    Dx9,
    Dx11,
    Unknown
};

enum class eGameVer
{
    SA,
    VC,
    III,
    SA_DE,
    VC_DE,
    III_DE,
    ReSA,
    ReVC,
    Re3,
    Unknown
};

extern eRenderer gRenderer;
extern eGameVer gGameVer;