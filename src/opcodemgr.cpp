#include "pch.h"
#include "opcodemgr.h"
#include <time.h>
#include "texturemgr.h"
#include "wrapper.hpp"
#include "imgui_internal.h"

static RTN_TYPE RUNTIME_API ImGuiBegin(RUNTIME_CONTEXT ctx) {
    char label[RUNTIME_STR_LEN];

    wGetStringWithFrame(ctx, label, RUNTIME_STR_LEN);
    bool openFlag = wGetBoolParam(ctx);
    bool noTitleBar = wGetBoolParam(ctx);
    bool noResize = wGetBoolParam(ctx);
    bool noMove = wGetBoolParam(ctx);
    bool autoResize = wGetBoolParam(ctx);

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
    if (noTitleBar) flags |= ImGuiWindowFlags_NoTitleBar;
    if (noResize) flags |= ImGuiWindowFlags_NoResize;
    if (noMove) flags |= ImGuiWindowFlags_NoMove;
    if (autoResize) flags |= ImGuiWindowFlags_AlwaysAutoResize;
    
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool isOpen = openFlag;
        ImGui::Begin(label, &isOpen, flags);
        data->SetData(label, 0, isOpen);
    };
    data->imgui.lastScriptCall = time(NULL);
    wSetIntParam(ctx, data->GetData(label, 0, true));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiButton(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    ImVec2 size;

    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool isPressed = ImGui::Button(buf, ImVec2(size.x, size.y));
        data->SetData(buf, 0, isPressed);
    };

    bool rtn = data->GetData(buf, 0, false);
    wUpdateCompareFlag(ctx, rtn);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiImageButton(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    ImVec2 size;

    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    TextureInfo *pInfo = reinterpret_cast<TextureInfo*>(wGetIntParam(ctx));
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);

    // FIX:
    // Due to an issue textures aren't loaded in LoadImage but on ImageButton first call

    if (!pInfo->pTexture && TextureMgr::Exists(pInfo)) {
        TextureMgr::LoadTexture(*pInfo);
    }

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PushID(buf);
        const ImGuiID id = ImGui::GetID(buf);
        bool isPressed = ImGui::ImageButtonEx(id, pInfo->pTexture, size, {0.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 0.0f}, data->imgui.m_ImGCol.m_fBgCol, data->imgui.m_ImGCol.m_fTintCol);
        ImGui::PopID();
        data->SetData(buf, 0, isPressed);
    };

    bool rtn = data->GetData(buf, 0, false);
    wUpdateCompareFlag(ctx, rtn);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetImageTintColor(RUNTIME_CONTEXT ctx) {
    ImVec4 col;
    col.x = wGetFloatParam(ctx);
    col.y = wGetFloatParam(ctx);
    col.z = wGetFloatParam(ctx);
    col.w = wGetFloatParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui.m_ImGCol.m_fTintCol = col;

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetImageBgColor(RUNTIME_CONTEXT ctx) {
    ImVec4 col;
    col.x = wGetFloatParam(ctx);
    col.y = wGetFloatParam(ctx);
    col.z = wGetFloatParam(ctx);
    col.w = wGetFloatParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui.m_ImGCol.m_fBgCol = col;

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiLoadImage(RUNTIME_CONTEXT ctx) {
    char fullPath[RUNTIME_STR_LEN*2], path[RUNTIME_STR_LEN];
    wGetStringParam(ctx, path, RUNTIME_STR_LEN);
    wResolvePath(ctx, path, fullPath, sizeof(fullPath));
    wSetIntParam(ctx, reinterpret_cast<int>(TextureMgr::LoadTextureFromPath(fullPath)));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiFreeImage(RUNTIME_CONTEXT ctx) {
    TextureInfo *pInfo = reinterpret_cast<TextureInfo*>(wGetIntParam(ctx));
    TextureMgr::FreeTexture(pInfo);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiArrowButton(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    int side = wGetIntParam(ctx);
    if (side < ImGuiDir_Left || side > ImGuiDir_Down) {
        wUpdateCompareFlag(ctx, false);
        return RTN_CONTINUE;
    }

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool isPressed = ImGui::ArrowButton(buf, side);
        data->SetData(buf, 0, isPressed);
    };

    bool rtn = data->GetData(buf, 0, false);
    wUpdateCompareFlag(ctx, rtn);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiInvisibleButton(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    ImVec2 size;

    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool isPressed = ImGui::InvisibleButton(buf, ImVec2(size.x, size.y));
        data->SetData(buf, 0, isPressed);
    };

    bool rtn = data->GetData(buf, 0, false);
    wUpdateCompareFlag(ctx, rtn);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiColorButton(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    ImVec4 rgba;
    ImVec2 size;

    // label, r, g, b, a, width, height
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    rgba.x = wGetFloatParam(ctx);
    rgba.y = wGetFloatParam(ctx);
    rgba.z = wGetFloatParam(ctx);
    rgba.w = wGetFloatParam(ctx);
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool isPressed = ImGui::ColorButton(buf, rgba, NULL, ImVec2(size.x, size.y));
        data->SetData(buf, 0, isPressed);
    };

    bool rtn = data->GetData(buf, 0, false);
    wUpdateCompareFlag(ctx, rtn);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiCheckbox(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    bool state = wGetBoolParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool check = state;

        bool clicked = ImGui::Checkbox(buf, &check);
        data->SetData(buf, 0, clicked);
        if (clicked) {
            data->SetData(buf, 1, check);
        }
    };

    bool clicked = data->GetData(buf, 0, state);
    wSetIntParam(ctx, clicked ? data->GetData(buf, 1, state) : state);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiEnd(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::End();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSameLine(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();

    data->imgui += [=]() {
        ImGui::SameLine();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiNewLine(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += []() {
        ImGui::NewLine();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiColumns(RUNTIME_CONTEXT ctx) {
    int count = wGetIntParam(ctx);
    count = max(count, 1);
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::Columns(count, NULL, false);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetColumnWidth(RUNTIME_CONTEXT ctx) {
    int idx = wGetIntParam(ctx);
    float width = wGetFloatParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetColumnWidth(idx, width);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiNextColumn(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::NextColumn();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSpacing(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::Spacing();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSeparator(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::Separator();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetFramerate(RUNTIME_CONTEXT ctx) {
    wSetIntParam(ctx, ScriptExData::GetGameFPS());
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetVersion(RUNTIME_CONTEXT ctx) {
    char* buf = const_cast<char*>(ImGui::GetVersion());
    unsigned char len = static_cast<unsigned char>(strlen(buf));
    wSetStringParam(ctx, buf);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetPluginVersion(RUNTIME_CONTEXT ctx) {
    wSetFloatParam(ctx, IMGUI_REDUX_VERSION);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetNextWindowPos(RUNTIME_CONTEXT ctx) {
    ImVec2 pos;
    pos.x = wGetFloatParam(ctx);
    pos.y = wGetFloatParam(ctx);
    int cond = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetNextWindowPos(pos, cond);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetNextWindowTransparency(RUNTIME_CONTEXT ctx) {
    float alpha = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetNextWindowBgAlpha(alpha);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetWindowPos(RUNTIME_CONTEXT ctx) {
    ImVec2 pos;
    pos.x = wGetFloatParam(ctx);
    pos.y = wGetFloatParam(ctx);
    int cond = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetWindowPos(pos, cond);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetNextWindowSize(RUNTIME_CONTEXT ctx) {
    ImVec2 size;
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);
    int cond = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetNextWindowSize(size, cond);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetWindowSize(RUNTIME_CONTEXT ctx) {
    ImVec2 size;
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);
    int cond = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetWindowSize(size, cond);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiDummy(RUNTIME_CONTEXT ctx) {
    ImVec2 size;
    size.x = wGetFloatParam(ctx);
    size.y = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::Dummy(size);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiText(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::TextUnformatted(buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiTextCentered(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::NewLine();
        ImVec2 size = ImGui::CalcTextSize(buf);
        float width = ImGui::GetWindowContentRegionWidth() - size.x;
        ImGui::SameLine(width/2);
        ImGui::TextUnformatted(buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiTextDisabled(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::TextDisabled(buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiTextWrapped(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::TextWrapped(buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiBulletText(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::BulletText(buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiTextColored(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);
    ImVec4 col;
    col.x = wGetFloatParam(ctx);
    col.y = wGetFloatParam(ctx);
    col.z = wGetFloatParam(ctx);
    col.w = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::TextColored(col, buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetTooltip(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringParam(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::SetTooltip(buf);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetCursorVisible(RUNTIME_CONTEXT ctx) {
    bool flag = wGetBoolParam(ctx);

    // Only update if cursor needs to be shown
    // Hidden by default
    if (flag) {
        ScriptExData* data = ScriptExData::Get();
        data->imgui += [=]() {
            data->SetCursorVisible(flag);
        };
    }

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetFrameHeight(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        data->SetData("__frameHeight__", 0, ImGui::GetFrameHeight());
    };
    wSetFloatParam(ctx, data->GetData("__frameHeight__", 0, 0.0f));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetWindowSize(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImVec2 size = ImGui::GetWindowSize();
        data->SetData(buf, 0, size.x);
        data->SetData(buf, 1, size.y);
    };

    ImVec2 size = { data->GetData(buf, 0, 0.0f), data->GetData(buf, 1, 0.0f) };
    wSetFloatParam(ctx, size.x);
    wSetFloatParam(ctx, size.y);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetDisplaySize(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    MONITORINFO info = {sizeof(info)};
    GetMonitorInfo(MonitorFromWindow(NULL, MONITOR_DEFAULTTOPRIMARY), &info);

    wSetFloatParam(ctx, info.rcMonitor.right - info.rcMonitor.left);
    wSetFloatParam(ctx, info.rcMonitor.bottom - info.rcMonitor.top);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetWindowPos(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImVec2 pos = ImGui::GetWindowPos();
        data->SetData(buf, 0, pos.x);
        data->SetData(buf, 1, pos.y);
    };

    ImVec2 pos = { data->GetData(buf, 0, 0.0f), data->GetData(buf, 1, 0.0f) };
    wSetFloatParam(ctx, pos.x);
    wSetFloatParam(ctx, pos.y);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiCalcTextSize(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImVec2 size = ImGui::CalcTextSize(buf);
        data->SetData(buf, 0, size.x);
        data->SetData(buf, 1, size.y);
    };

    ImVec2 size = { data->GetData(buf, 0, 0.0f), data->GetData(buf, 1, 0.0f) };
    wSetFloatParam(ctx, size.x);
    wSetFloatParam(ctx, size.y);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetWindowContentRegionWidth(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        float width = ImGui::GetWindowContentRegionWidth();
        data->SetData(buf, 0, width);
    };

    wSetFloatParam(ctx, data->GetData(buf, 0, 0.0f));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiBeginMainMenuBar(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool state = ImGui::BeginMainMenuBar();
        data->SetData(buf, 0, state);
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiEndMainMenuBar(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::EndMainMenuBar();
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiMenuItem(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    bool selected = wGetBoolParam(ctx);
    bool enabled = wGetBoolParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool state = ImGui::MenuItem(buf, NULL, selected, enabled);
        data->SetData(buf, 0, state);
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSelectable(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    bool selected = wGetBoolParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool state = ImGui::Selectable(buf, &selected);
        data->SetData(buf, 0, state);
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiBeginChild(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::BeginChild(buf);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiBeginChildEx(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    float szX = 0, szY = 0;
    bool border = false;
    int flags = 0;
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    szX = wGetFloatParam(ctx);
    szY = wGetFloatParam(ctx);
    border = wGetIntParam(ctx);
    flags = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::BeginChild(buf, {szX, szY}, border, flags);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiEndChild(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::EndChild();
    };

    return RTN_CONTINUE;
}


static RTN_TYPE RUNTIME_API ImGuiPushItemWidth(RUNTIME_CONTEXT ctx) {
    float width = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PushItemWidth(width);
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiPopItemWidth(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PopItemWidth();
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiCollapsingHeader(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool state = ImGui::CollapsingHeader(buf);
        data->SetData(buf, 0, state);
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetWidgetInt(RUNTIME_CONTEXT ctx) {
    char id[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, id, RUNTIME_STR_LEN);
    int val = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->SetData(id, 0, val);

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetWidgetFloat(RUNTIME_CONTEXT ctx) {
    char id[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, id, RUNTIME_STR_LEN);
    float val = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->SetData(id, 0, val);

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetWidgetText(RUNTIME_CONTEXT ctx) {
    char id[RUNTIME_STR_LEN], text[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, id, RUNTIME_STR_LEN);
    wGetStringWithFrame(ctx, text, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->SetData(id, 0, text);

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSliderInt(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    int initVal = wGetIntParam(ctx);
    int min = wGetIntParam(ctx);
    int max = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        int value = data->GetData(buf, 0, initVal);
        ImGui::SliderInt(buf, &value, min, max);
        data->SetData(buf, 0, value);
    };

    int value = data->GetData(buf, 0, initVal);
    wSetIntParam(ctx, value);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSliderFloat(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    float initVal = wGetFloatParam(ctx);
    float min = wGetFloatParam(ctx);
    float max = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        float value = data->GetData(buf, 0, initVal);
        ImGui::SliderFloat(buf, &value, min, max);
        data->SetData(buf, 0, value);
    };

    float value = data->GetData(buf, 0, initVal);
    wSetFloatParam(ctx, value);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiInputFloat(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    float initVal = wGetFloatParam(ctx);
    float min = wGetFloatParam(ctx);
    float max = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        float value = data->GetData(buf, 0, initVal);
        if (ImGui::InputFloat(buf, &value)) {
            if (value < min) {
                value = min;
            }

            if (value > max) {
                value = max;
            }
        }

        data->SetData(buf, 0, value);
    };

    float value = data->GetData(buf, 0, initVal);
    wSetFloatParam(ctx, value);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiInputInt(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    int initVal = wGetIntParam(ctx);
    int min = wGetIntParam(ctx);
    int max = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        int value = data->GetData(buf, 0, initVal);
        if (ImGui::InputInt(buf, &value)) {
            if (value < min) {
                value = min;
            }

            if (value > max) {
                value = max;
            }
        }

        data->SetData(buf, 0, value);
    };

    int value = data->GetData(buf, 0, initVal);
    wSetIntParam(ctx, value);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiInputText(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        std::string str = data->GetData(buf, 0, std::string(""));
        if (ImGui::InputText(buf, &str)) {
            data->SetData(buf, 0, str);
        }
    };

    std::string value = data->GetData(buf, 0, std::string(""));
    wSetStringParam(ctx, static_cast<char*>(&value[0]));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiColorPicker(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    ScriptExData* data = ScriptExData::Get();

    data->imgui += [=]() {
        float col[4];
        col[0] = data->GetData(buf, 0, 0) / 255.0f;
        col[1] = data->GetData(buf, 1, 0) / 255.0f;
        col[2] = data->GetData(buf, 2, 0) / 255.0f;
        col[3] = data->GetData(buf, 3, 0) / 255.0f;

        ImGui::ColorEdit4(buf, reinterpret_cast<float*>(&col), ImGuiColorEditFlags_AlphaBar);

        data->SetData(buf, 0, static_cast<int>(col[0] * 255));
        data->SetData(buf, 1, static_cast<int>(col[1] * 255));
        data->SetData(buf, 2, static_cast<int>(col[2] * 255));
        data->SetData(buf, 3, static_cast<int>(col[3] * 255));
    };

    int r = data->GetData(buf, 0, 0);
    int g = data->GetData(buf, 1, 0);
    int b = data->GetData(buf, 2, 0);
    int a = data->GetData(buf, 3, 0);

    wSetIntParam(ctx, r);
    wSetIntParam(ctx, g);
    wSetIntParam(ctx, b);
    wSetIntParam(ctx, a);
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiRadioButton(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    int curSelectedBtn = wGetIntParam(ctx);
    int btnNo = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();

    data->imgui += [=]() {
        int value = curSelectedBtn;
        bool clicked = ImGui::RadioButton(buf, &value, btnNo);
        data->SetData(buf, 0, clicked);

        if (clicked) {
            data->SetData(buf, 1, btnNo);
        } else {
            data->SetData(buf, 1, value);
        }
    };

    bool clicked = data->GetData(buf, 0, false);
    int value = data->GetData(buf, 1, 0);

    if (clicked) {
        wSetIntParam(ctx, value);
    } else {
        wSetIntParam(ctx, curSelectedBtn);
    }
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiBeginFrame(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    ScriptExData::SetCurrentScript(std::string(buf));
    ScriptExData *data = ScriptExData::Get();

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiEndFrame(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui.m_bRender = true;
    ScriptExData::SetCurrentScript("");

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiCombo(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN], options[256];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    // fill
    for (unsigned i = 0; i < 255; ++i) {
        options[i] = '\0';
    }

    wGetStringParam(ctx, options, 255);

    for (unsigned i = 0; i < 255; ++i) {
        if (options[i] == ',') {
            options[i] = '\0';
        }
    }
    int selectedOption = wGetIntParam(ctx);
    ScriptExData* data = ScriptExData::Get();

    data->imgui += [=]() {
        int value = selectedOption;
        bool clicked = ImGui::Combo(buf, &value, options);
        data->SetData(buf, 0, clicked);

        if(clicked) {
            data->SetData(buf, 1, value);
        } else {
            data->SetData(buf, 1, selectedOption);
        }
    };

    bool clicked = data->GetData(buf, 0, false);
    int value = data->GetData(buf, 1, 0);

    if (clicked) {
        wSetIntParam(ctx, value);
    } else {
        wSetIntParam(ctx, selectedOption);
    }
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiIsItemActive(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        data->SetData(buf, 0, ImGui::IsItemActive());
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiIsItemHovered(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        data->SetData(buf, 0, ImGui::IsItemHovered());
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, 0));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiIsItemClicked(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        data->SetData(buf, 0, ImGui::IsItemClicked());
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiIsItemFocused(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        data->SetData(buf, 0, ImGui::IsItemFocused());
    };

    wUpdateCompareFlag(ctx, data->GetData(buf, 0, false));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiBullet(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::Bullet();
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiSetMessage(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);

    // Remove everything after #
    // Also split the text if longer than window width
    float windowWidth = ImGui::GetIO().DisplaySize.x / 5.0f;
    const char* _start = buf;
    for (size_t i = 0; i < RUNTIME_STR_LEN; ++i) {
        if (buf[i] == '\0') break;

        if (buf[i] == '#') {
            buf[i] = '\0';
            break;
        }

        // Split the text into multiple lines if it exceeds window width
        if (buf[i] == ' ') {
            ImVec2 size = ImGui::CalcTextSize(_start, &buf[i]);

            if (size.x > windowWidth) {
                _start = &buf[i]+1; // increment the starting pos
                buf[i] = '\n';
            } else {
                buf[i] = ' ';
            }
        }
    }

    NotifiyPopup::AddToQueue(std::string(buf));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetScalingSize(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    int count = wGetIntParam(ctx);
    int spacing = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        bool spcaing_ = spacing;
        if (count == 1) {
            spcaing_ = false;
        }

        float factor = ImGui::GetStyle().ItemSpacing.x / 2.0f;
        float x, y;

        if (count == 3) {
            factor = ImGui::GetStyle().ItemSpacing.x / 1.403f;
        }

        if (spcaing_) {
            x = ImGui::GetWindowContentRegionWidth() / count - factor;
        } else {
            x = ImGui::GetWindowContentRegionWidth() / count;
        }

        y = ImGui::GetFrameHeight() * 1.3f;

        data->SetData(buf, 0, x);
        data->SetData(buf, 1, y);
    };

    wSetFloatParam(ctx, data->GetData(buf, 0, 10.0f));
    wSetFloatParam(ctx, data->GetData(buf, 1, 10.0f));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiPushStyleVar(RUNTIME_CONTEXT ctx) {
    int idx = wGetIntParam(ctx);
    float val = wGetFloatParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PushStyleVar(idx, val);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiPushStyleVar2(RUNTIME_CONTEXT ctx) {
    int idx = wGetIntParam(ctx);
    float x = wGetFloatParam(ctx);
    float y = wGetFloatParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PushStyleVar(idx, ImVec2(x, y));
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiPopStyleVar(RUNTIME_CONTEXT ctx) {
    int count = wGetIntParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PopStyleVar(count);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiPushStyleColor(RUNTIME_CONTEXT ctx) {
    int idx = wGetIntParam(ctx);
    int r = wGetIntParam(ctx);
    int g = wGetIntParam(ctx);
    int b = wGetIntParam(ctx);
    int a = wGetIntParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PushStyleColor(idx, IM_COL32(r, g, b, a));
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiPopStyleColor(RUNTIME_CONTEXT ctx) {
    int count = wGetIntParam(ctx);
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImGui::PopStyleColor(count);
    };
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetWindowDrawList(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImDrawList *drawList = ImGui::GetWindowDrawList();
        data->SetData("__WindowDrawlist__", 0, reinterpret_cast<int>(drawList));
    };

    wSetIntParam(ctx, data->GetData("__WindowDrawlist__", 0, 0));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetBackgroundDrawList(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImDrawList *drawList = ImGui::GetBackgroundDrawList();
        data->SetData("__BackgroundDrawlist__", 0, reinterpret_cast<int>(drawList));
    };

    wSetIntParam(ctx, data->GetData("__BackgroundDrawlist__", 0, 0));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiGetForegroundDrawList(RUNTIME_CONTEXT ctx) {
    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        ImDrawList *drawList = ImGui::GetForegroundDrawList();
        data->SetData("__ForegroundDrawlist__", 0, reinterpret_cast<int>(drawList));
    };

    wSetIntParam(ctx, data->GetData("__ForegroundDrawlist__", 0, 0));
    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiDrawListAddText(RUNTIME_CONTEXT ctx) {
    ImVec2 pos;
    ImVec4 col;
    ImDrawList *pDrawList = reinterpret_cast<ImDrawList*>(wGetIntParam(ctx));
    pos.x = wGetFloatParam(ctx);
    pos.y = wGetFloatParam(ctx);

    col.x = wGetIntParam(ctx) / 255.0f;
    col.y = wGetIntParam(ctx) / 255.0f;
    col.z = wGetIntParam(ctx) / 255.0f;
    col.w = wGetIntParam(ctx) / 255.0f;

    char text[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, text, RUNTIME_STR_LEN);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        if (pDrawList) {
            pDrawList->AddText(pos, IM_COL32(col.x, col.y, col.z, col.w), text);
        }
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiDrawListAddLine(RUNTIME_CONTEXT ctx) {
    ImVec2 p1, p2;
    ImVec4 col;
    ImDrawList *pDrawList = reinterpret_cast<ImDrawList*>(wGetIntParam(ctx));
    p1.x = wGetFloatParam(ctx);
    p1.y = wGetFloatParam(ctx);
    p2.x = wGetFloatParam(ctx);
    p2.y = wGetFloatParam(ctx);

    col.x = wGetIntParam(ctx) / 255.0f;
    col.y = wGetIntParam(ctx) / 255.0f;
    col.z = wGetIntParam(ctx) / 255.0f;
    col.w = wGetIntParam(ctx) / 255.0f;

    float thickness = wGetFloatParam(ctx);

    ScriptExData* data = ScriptExData::Get();
    data->imgui += [=]() {
        if (pDrawList) {
            pDrawList->AddLine(p1, p2, IM_COL32(col.x, col.y, col.z, col.w), thickness);
        }
    };

    return RTN_CONTINUE;
}

static RTN_TYPE RUNTIME_API ImGuiTabs(RUNTIME_CONTEXT ctx) {
    char buf[RUNTIME_STR_LEN], itemsBuf[RUNTIME_STR_LEN];
    wGetStringWithFrame(ctx, buf, RUNTIME_STR_LEN);
    ScriptExData* data = ScriptExData::Get();
    wGetStringParam(ctx, itemsBuf, RUNTIME_STR_LEN);
    std::vector<std::string> items;

    std::string temp;
    for (auto &c: itemsBuf) {
        if (c == ',' || c == '\0') {
            items.push_back(temp);
            if (c == '\0') {
                break;
            } else {
                temp = "";
            }
        } else {
            temp += c;
        }
    }

    data->imgui += [=]() {
        if (ImGui::BeginTabBar(buf)) {
            for (int i = 0; i < static_cast<int>(items.size()); ++i) {
                if (ImGui::BeginTabItem(items[i].c_str())) {
                    data->SetData(buf, 0, i);
                    ImGui::EndTabItem();
                }
            }
            ImGui::EndTabBar();
        }
    };

    int val = data->GetData(buf, 0, 0);
    wSetIntParam(ctx, val);
    return RTN_CONTINUE;
}

void OpcodeMgr::RegisterCommands() {

    // Note: Calling order can't be changed!
    wRegisterCommand("IMGUI_BEGIN_FRAME", ImGuiBeginFrame);
    wRegisterCommand("IMGUI_END_FRAME", ImGuiEndFrame);

    wRegisterCommand("IMGUI_BEGIN", ImGuiBegin);
    wRegisterCommand("IMGUI_END", ImGuiEnd);

    wRegisterCommand("IMGUI_BEGIN_MAINMENUBAR", ImGuiBeginMainMenuBar);
    wRegisterCommand("IMGUI_END_MAINMENUBAR", ImGuiEndMainMenuBar);

    wRegisterCommand("IMGUI_BEGIN_CHILD", ImGuiBeginChild);
    wRegisterCommand("IMGUI_END_CHILD", ImGuiEndChild);

    wRegisterCommand("IMGUI_TABS", ImGuiTabs);
    wRegisterCommand("IMGUI_COLLAPSING_HEADER", ImGuiCollapsingHeader);

    wRegisterCommand("IMGUI_SET_WINDOW_POS", ImGuiSetWindowPos);
    wRegisterCommand("IMGUI_SET_WINDOW_SIZE", ImGuiSetWindowSize);
    wRegisterCommand("IMGUI_SET_NEXT_WINDOW_POS", ImGuiSetNextWindowPos);
    wRegisterCommand("IMGUI_SET_NEXT_WINDOW_SIZE", ImGuiSetNextWindowSize);
    
    wRegisterCommand("IMGUI_TEXT", ImGuiText);
    wRegisterCommand("IMGUI_TEXT_CENTERED", ImGuiTextCentered);
    wRegisterCommand("IMGUI_TEXT_DISABLED", ImGuiTextDisabled);
    wRegisterCommand("IMGUI_TEXT_WRAPPED", ImGuiTextWrapped);
    wRegisterCommand("IMGUI_TEXT_COLORED", ImGuiTextColored);
    wRegisterCommand("IMGUI_BULLET_TEXT", ImGuiBulletText);
    
    wRegisterCommand("IMGUI_BULLET", ImGuiBullet);
    wRegisterCommand("IMGUI_CHECKBOX", ImGuiCheckbox);
    wRegisterCommand("IMGUI_COMBO", ImGuiCombo);
    wRegisterCommand("IMGUI_SET_TOOLTIP", ImGuiSetTooltip);

    wRegisterCommand("IMGUI_BUTTON", ImGuiButton);
    wRegisterCommand("IMGUI_IMAGE_BUTTON", ImGuiImageButton);
    wRegisterCommand("IMGUI_INVISIBLE_BUTTON", ImGuiInvisibleButton);
    wRegisterCommand("IMGUI_COLOR_BUTTON", ImGuiColorButton);
    wRegisterCommand("IMGUI_ARROW_BUTTON", ImGuiArrowButton);
    
    wRegisterCommand("IMGUI_SLIDER_INT", ImGuiSliderInt);
    wRegisterCommand("IMGUI_SLIDER_FLOAT", ImGuiSliderFloat);
    wRegisterCommand("IMGUI_INPUT_INT", ImGuiInputInt);
    wRegisterCommand("IMGUI_INPUT_FLOAT", ImGuiInputFloat);
    wRegisterCommand("IMGUI_INPUT_TEXT", ImGuiInputText);
    wRegisterCommand("IMGUI_RADIO_BUTTON", ImGuiRadioButton);
    wRegisterCommand("IMGUI_COLOR_PICKER", ImGuiColorPicker);
    wRegisterCommand("IMGUI_MENU_ITEM", ImGuiMenuItem);
    wRegisterCommand("IMGUI_SELECTABLE", ImGuiSelectable);

    wRegisterCommand("IMGUI_DUMMY", ImGuiDummy);
    wRegisterCommand("IMGUI_SAMELINE", ImGuiSameLine);
    wRegisterCommand("IMGUI_NEWLINE", ImGuiNewLine);
    wRegisterCommand("IMGUI_COLUMNS", ImGuiColumns);
    wRegisterCommand("IMGUI_NEXT_COLUMN", ImGuiNextColumn);
    wRegisterCommand("IMGUI_SPACING", ImGuiSpacing);
    wRegisterCommand("IMGUI_SEPARATOR", ImGuiSeparator);

    wRegisterCommand("IMGUI_PUSH_ITEM_WIDTH", ImGuiPushItemWidth);
    wRegisterCommand("IMGUI_POP_ITEM_WIDTH", ImGuiPopItemWidth);
    wRegisterCommand("IMGUI_IS_ITEM_ACTIVE", ImGuiIsItemActive);
    wRegisterCommand("IMGUI_IS_ITEM_CLICKED", ImGuiIsItemClicked);
    wRegisterCommand("IMGUI_IS_ITEM_FOCUSED", ImGuiIsItemFocused);
    wRegisterCommand("IMGUI_IS_ITEM_HOVERED", ImGuiIsItemHovered);
    wRegisterCommand("IMGUI_SET_ITEM_INT", ImGuiSetWidgetInt);
    wRegisterCommand("IMGUI_SET_ITEM_FLOAT", ImGuiSetWidgetFloat);
    wRegisterCommand("IMGUI_SET_ITEM_TEXT", ImGuiSetWidgetText);
 
    wRegisterCommand("IMGUI_SET_IMAGE_BG_COLOR", ImGuiSetImageBgColor);
    wRegisterCommand("IMGUI_SET_IMAGE_TINT_COLOR", ImGuiSetImageTintColor);
    wRegisterCommand("IMGUI_LOAD_IMAGE", ImGuiLoadImage);
    wRegisterCommand("IMGUI_FREE_IMAGE", ImGuiFreeImage);

    wRegisterCommand("IMGUI_PUSH_STYLE_VAR", ImGuiPushStyleVar);
    wRegisterCommand("IMGUI_PUSH_STYLE_VAR2", ImGuiPushStyleVar2);
    wRegisterCommand("IMGUI_PUSH_STYLE_COLOR", ImGuiPushStyleColor);
    wRegisterCommand("IMGUI_POP_STYLE_VAR", ImGuiPopStyleVar);
    wRegisterCommand("IMGUI_POP_STYLE_COLOR", ImGuiPopStyleColor);
 
    wRegisterCommand("IMGUI_GET_FOREGROUND_DRAWLIST", ImGuiGetForegroundDrawList);
    wRegisterCommand("IMGUI_GET_BACKGROUND_DRAWLIST", ImGuiGetBackgroundDrawList);
    wRegisterCommand("IMGUI_GET_WINDOW_DRAWLIST", ImGuiGetWindowDrawList);
    wRegisterCommand("IMGUI_DRAWLIST_ADD_TEXT", ImGuiDrawListAddText);
    wRegisterCommand("IMGUI_DRAWLIST_ADD_LINE", ImGuiDrawListAddLine);

    wRegisterCommand("GET_FRAMERATE", ImGuiGetFramerate);
    wRegisterCommand("IMGUI_GET_VERSION", ImGuiGetVersion);
    wRegisterCommand("IMGUI_GET_PLUGIN_VERSION", ImGuiGetPluginVersion);
    wRegisterCommand("IMGUI_SET_CURSOR_VISIBLE", ImGuiSetCursorVisible);
    wRegisterCommand("IMGUI_GET_FRAME_HEIGHT", ImGuiGetFrameHeight);
    wRegisterCommand("IMGUI_GET_WINDOW_POS", ImGuiGetWindowPos);
    wRegisterCommand("IMGUI_GET_WINDOW_SIZE", ImGuiGetWindowSize);
    wRegisterCommand("IMGUI_CALC_TEXT_SIZE", ImGuiCalcTextSize);
    wRegisterCommand("IMGUI_GET_WINDOW_CONTENT_REGION_WIDTH", ImGuiGetWindowContentRegionWidth);
    wRegisterCommand("IMGUI_GET_SCALING_SIZE", ImGuiGetScalingSize);
    wRegisterCommand("IMGUI_GET_DISPLAY_SIZE", ImGuiGetDisplaySize);
    wRegisterCommand("IMGUI_SET_NEXT_WINDOW_TRANSPARENCY", ImGuiSetNextWindowTransparency);
    wRegisterCommand("IMGUI_SET_MESSAGE", ImGuiSetMessage);
    wRegisterCommand("IMGUI_SETCOLUMN_WIDTH", ImGuiSetColumnWidth);
    wRegisterCommand("IMGUI_BEGIN_CHILD", ImGuiBeginChildEx);
}
