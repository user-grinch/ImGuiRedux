#include "pch.h"
#include "opcodemgr.h"

static HandlerResult ImGuiBegin(Context ctx)
{
	char label[STR_MAX_LEN];
	bool closedFlag;
	int windowFlags;
	int result;

	GetStringParam(ctx, label, STR_MAX_LEN);
	closedFlag = (GetIntParam(ctx) == 0);
	windowFlags = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		data->frame_cache[label] = ImGui::Begin(label, (bool*)&closedFlag, windowFlags);
	};
	UpdateCompareFlag(ctx, data->frame_cache[label]);

	return HR_CONTINUE;
}

static HandlerResult ImGuiButton(Context ctx)
{
	char buf[STR_MAX_LEN];
	ImVec2 size;

	GetStringParam(ctx, buf, STR_MAX_LEN);
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		data->frame_cache[buf] = ImGui::Button(buf, ImVec2(size.x, size.y));
	};
	UpdateCompareFlag(ctx, data->frame_cache[buf]);

	return HR_CONTINUE;
}
//
//static HandlerResult ImGuiCheckbox(Context ctx)
//{
//	char buf[STR_MAX_LEN];
//	ImVec2 size;
//
//	GetStringParam(ctx, buf, STR_MAX_LEN);
//	size.x = GetFloatParam(ctx);
//	size.y = GetFloatParam(ctx);
//
//	ScriptExData* data = ScriptExData::Get(&ctx);
//	data->imgui += [=]()
//	{
//		data->frame_cache[buf] = ImGui::Button(buf, ImVec2(size.x, size.y));
//	};
//	UpdateCompareFlag(ctx, data->frame_cache[buf]);
//
//	return HR_CONTINUE;
//}
//
//OpcodeResult WINAPI ImGuiCheckbox2(CScriptThread* thread)
//{
//	char buf[BUFFER_SIZE];
//	CLEO_ReadStringPointerOpcodeParam(thread, buf, sizeof(buf));
//
//	int* var = nullptr;
//	Util::GetProperTypeData(thread, var);
//
//	ScriptExData* data = ScriptExData::Get(thread);
//
//	data->imgui += [=]()
//	{
//		data->frame_cache[buf] = ImGui::Checkbox(buf, (bool*)var);
//	};
//
//	reinterpret_cast<CRunningScript*>(thread)->UpdateCompareFlag(data->frame_cache[buf]);
//	return OR_CONTINUE;
//}


static HandlerResult ImGuiEnd(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += []()
	{
		ImGui::End();
	};
	return HR_CONTINUE;
}

void OpcodeMgr::RegisterCommands()
{
	RegisterCommand("IMGUI_BEGIN", ImGuiBegin, "imgui");
	RegisterCommand("IMGUI_END", ImGuiEnd, "imgui");
	RegisterCommand("IMGUI_BUTTON", ImGuiButton, "imgui");
}
