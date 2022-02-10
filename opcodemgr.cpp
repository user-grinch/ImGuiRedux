#include "pch.h"
#include "opcodemgr.h"

static HandlerResult ImGuiBegin(Context ctx)
{
	char label[STR_MAX_LEN];
	bool closedFlag;
	int windowFlags;

	GetStringParam(ctx, label, STR_MAX_LEN);
	closedFlag = (GetIntParam(ctx) == 0);
	windowFlags = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		bool isClosed;
		bool rtn = ImGui::Begin(label, &isClosed, windowFlags);

		data->SetData(label, 0, rtn);
		data->SetData(label, 1, isClosed);
	};

	bool rtn = data->GetData(label, 0, false);
	bool openFlag = data->GetData(label, 1, false);

	SetIntParam(ctx, rtn);
	SetIntParam(ctx, openFlag);
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
		bool isPressed = ImGui::Button(buf, ImVec2(size.x, size.y));
		data->SetData(buf, 0, isPressed);
	};

	bool rtn = data->GetData(buf, 0, false);
	UpdateCompareFlag(ctx, rtn);
	return HR_CONTINUE;
}

static HandlerResult ImGuiInvisibleButton(Context ctx)
{
	char buf[STR_MAX_LEN];
	ImVec2 size;

	GetStringParam(ctx, buf, STR_MAX_LEN);
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		bool isPressed = ImGui::InvisibleButton(buf, ImVec2(size.x, size.y));
		data->SetData(buf, 0, isPressed);
	};

	bool rtn = data->GetData(buf, 0, false);
	UpdateCompareFlag(ctx, rtn);
	return HR_CONTINUE;
}

static HandlerResult ImGuiColorButton(Context ctx)
{
	char buf[STR_MAX_LEN];
	ImVec4 rgba;
	ImVec2 size;
	int flags;

	// label, r, g, b, a, width, height
	GetStringParam(ctx, buf, STR_MAX_LEN);
	rgba.x = GetFloatParam(ctx);
	rgba.y = GetFloatParam(ctx);
	rgba.z = GetFloatParam(ctx);
	rgba.w = GetFloatParam(ctx);
	flags = GetIntParam(ctx);
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		bool isPressed = ImGui::ColorButton(buf, rgba, flags, ImVec2(size.x, size.y));
		data->SetData(buf, 0, isPressed);
	};

	bool rtn = data->GetData(buf, 0, false);
	UpdateCompareFlag(ctx, rtn);
	return HR_CONTINUE;
}

static HandlerResult ImGuiCheckbox(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);
	bool state = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		bool check = state;
		bool rtn = ImGui::Checkbox(buf, &check);
		data->SetData(buf, 0, rtn);
		data->SetData(buf, 1, check);
	};
	bool rtn = data->GetData(buf, 0, false);
	bool check = data->GetData(buf, 1, false);

	SetIntParam(ctx, rtn);
	SetIntParam(ctx, check);
	return HR_CONTINUE;
}

static HandlerResult ImGuiEnd(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += []()
	{
		ImGui::End();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSameLine(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += []()
	{
		ImGui::SameLine();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiNewLine(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += []()
	{
		ImGui::NewLine();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiColumns(Context ctx)
{
	int count = GetIntParam(ctx);
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::Columns(count, NULL, false);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiNextColumn(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::NextColumn();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSpacing(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::Spacing();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSeparator(Context ctx)
{
	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::Separator();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetFramerate(Context ctx)
{
	SetIntParam(ctx, OpcodeMgr::nFPS);
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetVersion(Context ctx)
{
	char* buf = (char*)ImGui::GetVersion();
	size_t len = strlen(buf);
	SetStringParam(ctx, buf, len);
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetReduxVersion(Context ctx)
{
	SetFloatParam(ctx, IMGUI_REDUX_VERSION);
	return HR_CONTINUE;
}

static HandlerResult ImGuiSetNextWindowPos(Context ctx)
{
	ImVec2 pos;
	pos.x = GetFloatParam(ctx);
	pos.y = GetFloatParam(ctx);
	int cond = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::SetNextWindowPos(pos, cond);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiSetWindowPos(Context ctx)
{
	ImVec2 pos;
	pos.x = GetFloatParam(ctx);
	pos.y = GetFloatParam(ctx);
	int cond = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::SetWindowPos(pos, cond);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiSetNextWindowSize(Context ctx)
{
	ImVec2 size;
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);
	int cond = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::SetNextWindowSize(size, cond);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiSetWindowSize(Context ctx)
{
	ImVec2 size;
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);
	int cond = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::SetWindowSize(size, cond);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiDummy(Context ctx)
{
	ImVec2 size;
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::Dummy(size);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiText(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::Text(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiTextDisabled(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::TextDisabled(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiTextWrapped(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::TextWrapped(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiBulletText(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::BulletText(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiTextColored(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);
	ImVec4 col;
	col.x = GetFloatParam(ctx);
	col.y = GetFloatParam(ctx);
	col.z = GetFloatParam(ctx);
	col.w = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::TextColored(col, buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSetTooltip(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get(&ctx);
	data->imgui += [=]()
	{
		ImGui::SetTooltip(buf);
	};
	return HR_CONTINUE;
}

void OpcodeMgr::RegisterCommands()
{
	RegisterCommand("IMGUI_BEGIN", ImGuiBegin, "imgui");
	RegisterCommand("IMGUI_END", ImGuiEnd, "imgui");

	RegisterCommand("IMGUI_SET_WINDOW_POS", ImGuiSetWindowPos, "imgui");
	RegisterCommand("IMGUI_SET_WINDOW_SIZE", ImGuiSetWindowPos, "imgui");
	RegisterCommand("IMGUI_SET_NEXT_WINDOW_POS", ImGuiSetNextWindowPos, "imgui");
	RegisterCommand("IMGUI_SET_NEXT_WINDOW_SIZE", ImGuiSetNextWindowPos, "imgui");
	RegisterCommand("IMGUI_DUMMY", ImGuiDummy, "imgui");

	RegisterCommand("IMGUI_TEXT", ImGuiText, "imgui");
	RegisterCommand("IMGUI_TEXT_DISABLED", ImGuiTextDisabled, "imgui");
	RegisterCommand("IMGUI_TEXT_WRAPPED", ImGuiTextWrapped, "imgui");
	RegisterCommand("IMGUI_TEXT_COLORED", ImGuiTextColored, "imgui");
	RegisterCommand("IMGUI_BULLET_TEXT", ImGuiBulletText, "imgui");
	RegisterCommand("IMGUI_SET_TOOLTIP", ImGuiSetTooltip, "imgui");

	RegisterCommand("IMGUI_BUTTON", ImGuiButton, "imgui");
	RegisterCommand("IMGUI_INVISIBLE_BUTTON", ImGuiInvisibleButton, "imgui");
	RegisterCommand("IMGUI_COLOR_BUTTON", ImGuiColorButton, "imgui");

	RegisterCommand("IMGUI_CHECKBOX", ImGuiCheckbox, "imgui");

	RegisterCommand("IMGUI_SAMELINE", ImGuiSameLine, "imgui");
	RegisterCommand("IMGUI_NEWLINE", ImGuiNewLine, "imgui");
	RegisterCommand("IMGUI_COLUMNS", ImGuiColumns, "imgui");
	RegisterCommand("IMGUI_NEXT_COLUMN", ImGuiNextColumn, "imgui");
	RegisterCommand("IMGUI_SPACING", ImGuiSpacing, "imgui");
	RegisterCommand("IMGUI_SEPARATOR", ImGuiSeparator, "imgui");

	RegisterCommand("IMGUI_GET_FRAMERATE", ImGuiGetFramerate, "imgui");
	RegisterCommand("IMGUI_GET_VERSION", ImGuiGetVersion, "imgui");
	RegisterCommand("IMGUI_GET_REDUX_VERSION", ImGuiGetReduxVersion, "imgui");
}
