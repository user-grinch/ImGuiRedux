#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "depend/cleo/cleo_redux_sdk.h"
#include "depend/imgui/imgui.h"
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