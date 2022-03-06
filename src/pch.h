#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define IMGUI_REDUX_VERSION 0.2f

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
    III,
    VC,
    SA,
    III_DE,
    VC_DE,
    SA_DE,
    Re3,
    ReVC,
    ReSA,
    Unknown
};

extern eRenderer gRenderer;
extern eGameVer gGameVer;