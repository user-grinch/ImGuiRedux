#include "pch.h"
#include "opcodemgr.h"

/*
	Appending BeginFrame uniqueID to widget names
	This ensures scripts won't clash with each other
*/
static void GetString(Context ctx, char* label, unsigned char length)
{
	GetStringParam(ctx, label, STR_MAX_LEN);
	strcat(label, "##");
	strcat(label, ScriptExData::GetCurrentScript().c_str());
}

static HandlerResult ImGuiBegin(Context ctx)
{
	char label[STR_MAX_LEN];
	bool closedFlag;
	int windowFlags;

	GetString(ctx, label, STR_MAX_LEN);
	closedFlag = (GetIntParam(ctx) == 0);
	windowFlags = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool isClosed;
		bool rtn = ImGui::Begin(label, &isClosed, windowFlags);
		data->SetData(label, 0, rtn);
		data->SetData(label, 1, isClosed);
	};

	bool rtn = data->GetData<bool>(label, 0, false);
	bool openFlag = data->GetData<bool>(label, 1, false);

	SetIntParam(ctx, rtn);
	SetIntParam(ctx, openFlag);
	return HR_CONTINUE;
}

static HandlerResult ImGuiButton(Context ctx)
{
	char buf[STR_MAX_LEN];
	ImVec2 size;

	GetString(ctx, buf, STR_MAX_LEN);
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
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

	GetString(ctx, buf, STR_MAX_LEN);
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
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

	// label, r, g, b, a, width, height
	GetString(ctx, buf, STR_MAX_LEN);
	rgba.x = GetFloatParam(ctx);
	rgba.y = GetFloatParam(ctx);
	rgba.z = GetFloatParam(ctx);
	rgba.w = GetFloatParam(ctx);
	size.x = GetFloatParam(ctx);
	size.y = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool isPressed = ImGui::ColorButton(buf, rgba, NULL, ImVec2(size.x, size.y));
		data->SetData(buf, 0, isPressed);
	};

	bool rtn = data->GetData(buf, 0, false);
	UpdateCompareFlag(ctx, rtn);
	return HR_CONTINUE;
}

static HandlerResult ImGuiCheckbox(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	bool state = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool check = state;

		if (ImGui::Checkbox(buf, &check))
		{
			data->SetData(buf, 0, check);
		}
		else
		{
			data->SetData(buf, 0, data->GetData(buf, 0, false));
		}
	};
	bool check = data->GetData(buf, 0, false);

	SetIntParam(ctx, check);
	return HR_CONTINUE;
}

static HandlerResult ImGuiEnd(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::End();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSameLine(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();

	data->imgui += [=]()
	{
		ImGui::SameLine();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiNewLine(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += []()
	{
		ImGui::NewLine();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiColumns(Context ctx)
{
	int count = GetIntParam(ctx);
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Columns(count, NULL, false);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiNextColumn(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::NextColumn();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSpacing(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Spacing();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSeparator(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Separator();
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetFramerate(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData("__framerate__", 0, (int)ImGui::GetIO().Framerate);
	};
	SetIntParam(ctx, data->GetData("__framerate__", 0, 0));
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetVersion(Context ctx)
{
	char* buf = (char*)ImGui::GetVersion();
	unsigned char len = (unsigned char)strlen(buf);
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

	ScriptExData* data = ScriptExData::Get();
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

	ScriptExData* data = ScriptExData::Get();
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

	ScriptExData* data = ScriptExData::Get();
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

	ScriptExData* data = ScriptExData::Get();
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

	ScriptExData* data = ScriptExData::Get();
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

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Text(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiTextDisabled(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::TextDisabled(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiTextWrapped(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::TextWrapped(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiBulletText(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::BulletText(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiTextColored(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	ImVec4 col;
	col.x = GetFloatParam(ctx);
	col.y = GetFloatParam(ctx);
	col.z = GetFloatParam(ctx);
	col.w = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::TextColored(col, buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSetTooltip(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::SetTooltip(buf);
	};
	return HR_CONTINUE;
}

static HandlerResult ImGuiSetCursorVisible(Context ctx)
{
	bool flag = GetIntParam(ctx);

	// Only update if cursor needs to be shown
	// Hidden by default
	if (flag)
	{
		ScriptExData* data = ScriptExData::Get();
		data->imgui += [=]()
		{
			data->SetCursorVisible(flag);
		};
	}
	
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetFrameHeight(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData("__frameHeight__", 0, ImGui::GetFrameHeight());
	};
	SetFloatParam(ctx, data->GetData("__frameHeight__", 0, 0.0f));
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetWindowSize(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImVec2 size = ImGui::GetWindowSize();
		data->SetData(buf, 0, size.x);
		data->SetData(buf, 1, size.y);
	};

	ImVec2 size = { data->GetData(buf, 0, 0.0f), data->GetData(buf, 1, 0.0f) };
	SetFloatParam(ctx, size.x);
	SetFloatParam(ctx, size.y);
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetWindowPos(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImVec2 pos = ImGui::GetWindowPos();
		data->SetData(buf, 0, pos.x);
		data->SetData(buf, 1, pos.y);
	};

	ImVec2 pos = { data->GetData(buf, 0, 0.0f), data->GetData(buf, 1, 0.0f) };
	SetFloatParam(ctx, pos.x);
	SetFloatParam(ctx, pos.y);
	return HR_CONTINUE;
}

static HandlerResult ImGuiCalcTextSize(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImVec2 size = ImGui::CalcTextSize(buf);
		data->SetData(buf, 0, size.x);
		data->SetData(buf, 1, size.y);
	};

	ImVec2 size = { data->GetData(buf, 0, 0.0f), data->GetData(buf, 1, 0.0f) };
	SetFloatParam(ctx, size.x);
	SetFloatParam(ctx, size.y);
	return HR_CONTINUE;
}

static HandlerResult ImGuiGetWindowContentRegionWidth(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		float width = ImGui::GetWindowContentRegionWidth();
		data->SetData(buf, 0, width);
	};

	SetFloatParam(ctx, data->GetData(buf, 0, 0.0f));
	return HR_CONTINUE;
}

static HandlerResult ImGuiBeginMainMenuBar(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool state = ImGui::BeginMainMenuBar();
		data->SetData(buf, 0, state);
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

static HandlerResult ImGuiEndMainMenuBar(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::EndMainMenuBar();
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiMenuItem(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	bool selected = (GetIntParam(ctx) != 0);
	bool enabled = (GetIntParam(ctx) != 0);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool state = ImGui::MenuItem(buf, NULL, selected, enabled);
		data->SetData(buf, 0, state);
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

static HandlerResult ImGuiSelectable(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	bool selected = (GetIntParam(ctx) != 0);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool state = ImGui::Selectable(buf, &selected);
		data->SetData(buf, 0, state);
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

static HandlerResult ImGuiBeginChild(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::BeginChild(buf);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiEndChild(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::EndChild();
	};

	return HR_CONTINUE;
}


static HandlerResult ImGuiPushItemWidth(Context ctx)
{
	float width = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::PushItemWidth(width);
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiPopItemWidth(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::PopItemWidth();
	};

	return HR_CONTINUE;
}

static HandlerResult ImGuiCollapsingHeader(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool state = ImGui::CollapsingHeader(buf);
		data->SetData(buf, 0, state);
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

static HandlerResult ImGuiSliderInt(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	int initVal = GetIntParam(ctx);
	int min = GetIntParam(ctx);
	int max = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		int value = data->GetData(buf, 0, initVal);
		ImGui::SliderInt(buf, &value, min, max);
		data->SetData(buf, 0, value);
	};

	int value = data->GetData(buf, 0, initVal);
	SetIntParam(ctx, value);
	return HR_CONTINUE;
}

static HandlerResult ImGuiSliderFloat(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	float initVal = GetFloatParam(ctx);
	float min = GetFloatParam(ctx);
	float max = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		float value = data->GetData(buf, 0, initVal);
		ImGui::SliderFloat(buf, &value, min, max);
		data->SetData(buf, 0, value);
	};

	float value = data->GetData(buf, 0, initVal);
	SetFloatParam(ctx, value);
	return HR_CONTINUE;
}

static HandlerResult ImGuiInputFloat(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	float initVal = GetFloatParam(ctx);
	float min = GetFloatParam(ctx);
	float max = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		float value = data->GetData(buf, 0, initVal);
		if (ImGui::InputFloat(buf, &value))
		{
			if (value < min) 
			{
				value = min;
			}

			if (value > max) 
			{
				value = max;
			}
		}

		data->SetData(buf, 0, value);
	};

	float value = data->GetData(buf, 0, initVal);
	SetFloatParam(ctx, value);
	return HR_CONTINUE;
}

static HandlerResult ImGuiInputInt(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	int initVal = GetIntParam(ctx);
	int min = GetIntParam(ctx);
	int max = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		int value = data->GetData(buf, 0, initVal);
		if (ImGui::InputInt(buf, &value))
		{
			if (value < min) 
			{
				value = min;
			}

			if (value > max) 
			{
				value = max;
			}
		}

		data->SetData(buf, 0, value);
	};

	int value = data->GetData(buf, 0, initVal);
	SetIntParam(ctx, value);
	return HR_CONTINUE;
}

static HandlerResult ImGuiInputText(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		std::string str = data->GetData(buf, 0, std::string(""));
		if (ImGui::InputText(buf, &str))
		{
			data->SetData(buf, 0, str);
		}
	};

	std::string value = data->GetData(buf, 0, std::string(""));
	SetStringParam(ctx, (char*)&value[0], (unsigned char)value.size());
	return HR_CONTINUE;
}

static HandlerResult ImGuiColorPicker(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	ScriptExData* data = ScriptExData::Get();

	data->imgui += [=]()
	{
		float col[4];
		col[0] = data->GetData(buf, 0, 0.0f);
		col[1] = data->GetData(buf, 1, 0.0f);
		col[2] = data->GetData(buf, 2, 0.0f);
		col[3] = data->GetData(buf, 3, 0.0f);

		ImGui::ColorEdit4(buf, (float*)&col);
		
		data->SetData(buf, 0, col[0]);
		data->SetData(buf, 1, col[1]);
		data->SetData(buf, 2, col[2]);
		data->SetData(buf, 3, col[3]);
	};

	float r = data->GetData(buf, 0, 0.0f);
	float g = data->GetData(buf, 1, 0.0f);
	float b = data->GetData(buf, 2, 0.0f);
	float a = data->GetData(buf, 3, 0.0f);

	SetFloatParam(ctx, r);
	SetFloatParam(ctx, g);
	SetFloatParam(ctx, b);
	SetFloatParam(ctx, a);
	return HR_CONTINUE;
}

static HandlerResult ImGuiRadioButton(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	int curSelectedBtn = GetIntParam(ctx);
	int btnNo = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get();

	data->imgui += [=]()
	{
		int value = curSelectedBtn;
		bool clicked = ImGui::RadioButton(buf, &value, btnNo);
		data->SetData(buf, 0, clicked);

		if (clicked)
		{
			data->SetData(buf, 1, btnNo);
		}
		else
		{
			data->SetData(buf, 1, value);
		}
	};

	bool clicked = data->GetData(buf, 0, false);
	int value = data->GetData(buf, 1, 0);

	if (clicked)
	{
		SetIntParam(ctx, value);
	}
	else
	{
		SetIntParam(ctx, curSelectedBtn);
	}
	return HR_CONTINUE;
}

static HandlerResult ImGuiBeginFrame(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	ScriptExData::SetCurrentScript(std::string(buf));

	return HR_CONTINUE;
}

static HandlerResult ImGuiEndFrame(Context ctx)
{
	ScriptExData::SetCurrentScript("");
	return HR_CONTINUE;
}

static HandlerResult ImGuiIsItemActive(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData(buf, 0, ImGui::IsItemActive());
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

static HandlerResult ImGuiIsItemHovered(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData(buf, 0, ImGui::IsItemHovered());
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	UpdateCompareFlag(ctx, data->GetData(buf, 0, 0));
	return HR_CONTINUE;
}

static HandlerResult ImGuiIsItemClicked(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData(buf, 0, ImGui::IsItemClicked());
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

static HandlerResult ImGuiIsItemFocused(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData(buf, 0, ImGui::IsItemFocused());
	};

	SetIntParam(ctx, data->GetData(buf, 0, false));
	return HR_CONTINUE;
}

void OpcodeMgr::RegisterCommands()
{
	RegisterCommand("IMGUI_BEGIN_FRAME", ImGuiBeginFrame, "imgui");
	RegisterCommand("IMGUI_END_FRAME", ImGuiEndFrame, "imgui");

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

	RegisterCommand("IMGUI_SET_CURSOR_VISIBLE", ImGuiSetCursorVisible, "imgui");
	RegisterCommand("IMGUI_GET_FRAME_HEIGHT", ImGuiGetFrameHeight, "imgui");
	RegisterCommand("IMGUI_GET_WINDOW_POS", ImGuiGetWindowPos, "imgui");
	RegisterCommand("IMGUI_GET_WINDOW_SIZE", ImGuiGetWindowSize, "imgui");
	RegisterCommand("IMGUI_CALC_TEXT_SIZE", ImGuiCalcTextSize, "imgui");
	RegisterCommand("IMGUI_GET_WINDOW_CONTENT_REGION_WIDTH", ImGuiGetWindowContentRegionWidth, "imgui");
	RegisterCommand("IMGUI_BEGIN_MAINMENUBAR", ImGuiBeginMainMenuBar, "imgui");
	RegisterCommand("IMGUI_END_MAINMENUBAR", ImGuiEndMainMenuBar, "imgui");
	RegisterCommand("IMGUI_MENU_ITEM", ImGuiMenuItem, "imgui");
	RegisterCommand("IMGUI_SELECTABLE", ImGuiSelectable, "imgui");
	RegisterCommand("IMGUI_BEGIN_CHILD", ImGuiBeginChild, "imgui");
	RegisterCommand("IMGUI_END_CHILD", ImGuiEndChild, "imgui");
	RegisterCommand("IMGUI_PUSH_ITEM_WIDTH", ImGuiPushItemWidth, "imgui");
	RegisterCommand("IMGUI_POP_ITEM_WIDTH", ImGuiPopItemWidth, "imgui");

	RegisterCommand("IMGUI_COLLAPSING_HEADER", ImGuiCollapsingHeader, "imgui");

	RegisterCommand("IMGUI_SLIDER_INT", ImGuiSliderInt, "imgui");
	RegisterCommand("IMGUI_SLIDER_FLOAT", ImGuiSliderFloat, "imgui");
	RegisterCommand("IMGUI_INPUT_INT", ImGuiInputInt, "imgui");
	RegisterCommand("IMGUI_INPUT_FLOAT", ImGuiInputFloat, "imgui");
	RegisterCommand("IMGUI_INPUT_TEXT", ImGuiInputText, "imgui");
	RegisterCommand("IMGUI_RADIO_BUTTON", ImGuiRadioButton, "imgui");
	RegisterCommand("IMGUI_COLOR_PICKER", ImGuiColorPicker, "imgui");

	RegisterCommand("IMGUI_IS_ITEM_ACTIVE", ImGuiIsItemActive, "imgui");
	RegisterCommand("IMGUI_IS_ITEM_CLICKED", ImGuiIsItemClicked, "imgui");
	RegisterCommand("IMGUI_IS_ITEM_FOCUSED", ImGuiIsItemFocused, "imgui");
	RegisterCommand("IMGUI_IS_ITEM_HOVERED", ImGuiIsItemHovered, "imgui");
}
