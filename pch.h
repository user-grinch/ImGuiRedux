#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define INJECTOR_GVM_PLUGIN_NAME "ImGuiRedux"
#define GTASA // temp

#ifdef GTASA
#define BY_GAME(sa, vc, iii) sa
#elif GTAVC
#define BY_GAME(sa, vc, iii) vc
#elif GTA3
#define BY_GAME(sa, vc, iii) iii
#endif

#include "depend/cleo/cleo_redux_sdk.h"
#include "plugin.h"
#include "depend/imgui/imgui.h"

using namespace plugin;

enum eRenderer
{
    Render_DirectX9,
    Render_DirectX11,
    Render_Unknown
};

static eRenderer gRenderer = Render_Unknown;