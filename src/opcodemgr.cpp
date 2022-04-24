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

	GetString(ctx, label, STR_MAX_LEN);
	bool closedFlag = GetIntParam(ctx);
	bool noTitleBar = GetIntParam(ctx);
	bool noResize = GetIntParam(ctx);
	bool noMove = GetIntParam(ctx);
	bool autoResize = GetIntParam(ctx);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse;
	if (noTitleBar) flags |= ImGuiWindowFlags_NoTitleBar;
	if (noResize) flags |= ImGuiWindowFlags_NoResize;
	if (noMove) flags |= ImGuiWindowFlags_NoMove;
	if (autoResize) flags |= ImGuiWindowFlags_AlwaysAutoResize;
		
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool isClosed = closedFlag;
		ImGui::Begin(label, &isClosed, flags);
		data->SetData(label, 0, isClosed);
	};

	SetIntParam(ctx, data->GetData<bool>(label, 0, false));
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiEnd(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::End();
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiSameLine(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();

	data->imgui += [=]()
	{
		ImGui::SameLine();
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiNewLine(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += []()
	{
		ImGui::NewLine();
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiColumns(Context ctx)
{
	int count = GetIntParam(ctx);
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Columns(count, NULL, false);
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiNextColumn(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::NextColumn();
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiSpacing(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Spacing();
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiSeparator(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::Separator();
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiGetFramerate(Context ctx)
{
	SetIntParam(ctx, ScriptExData::GetGameFPS());
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiGetVersion(Context ctx)
{
	char* buf = (char*)ImGui::GetVersion();
	unsigned char len = (unsigned char)strlen(buf);
	SetStringParam(ctx, buf);
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiGetPluginVersion(Context ctx)
{
	SetFloatParam(ctx, IMGUI_REDUX_VERSION);
	return HandlerResult::CONTINUE;
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

	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiSetNextWindowTransparency(Context ctx)
{
	float alpha = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::SetNextWindowBgAlpha(alpha);
	};

	return HandlerResult::CONTINUE;
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

	return HandlerResult::CONTINUE;
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

	return HandlerResult::CONTINUE;
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

	return HandlerResult::CONTINUE;
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

	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiTextDisabled(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::TextDisabled(buf);
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiTextWrapped(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::TextWrapped(buf);
	};
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiBulletText(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetStringParam(ctx, buf, STR_MAX_LEN);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::BulletText(buf);
	};
	return HandlerResult::CONTINUE;
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

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::TextColored(col, buf);
	};
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiGetFrameHeight(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		data->SetData("__frameHeight__", 0, ImGui::GetFrameHeight());
	};
	SetFloatParam(ctx, data->GetData("__frameHeight__", 0, 0.0f));
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiGetDisplaySize(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImVec2 size = ImGui::GetIO().DisplaySize;
		data->SetData("__displayX__", 0, size.x);
		data->SetData("__displayY__", 1, size.y);
	};

	ImVec2 size = { data->GetData("__displayX__", 0, 0.0f), data->GetData("__displayY__", 1, 0.0f) };
	SetFloatParam(ctx, size.x);
	SetFloatParam(ctx, size.y);
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiEndMainMenuBar(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::EndMainMenuBar();
	};

	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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

	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiEndChild(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::EndChild();
	};

	return HandlerResult::CONTINUE;
}


static HandlerResult ImGuiPushItemWidth(Context ctx)
{
	float width = GetFloatParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::PushItemWidth(width);
	};

	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiPopItemWidth(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		ImGui::PopItemWidth();
	};

	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	SetStringParam(ctx, (char*)&value[0]);
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiBeginFrame(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	ScriptExData::SetCurrentScript(std::string(buf));
	ScriptExData *data = ScriptExData::Get();
	
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiEndFrame(Context ctx)
{
	ScriptExData* data = ScriptExData::Get();
	data->imgui.m_bRender = true;
	ScriptExData::SetCurrentScript("");
	
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
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
	return HandlerResult::CONTINUE;
}

static HandlerResult ImGuiGetScalingSize(Context ctx)
{
	char buf[STR_MAX_LEN];
	GetString(ctx, buf, STR_MAX_LEN);
	int count = GetIntParam(ctx);
	int spacing = GetIntParam(ctx);

	ScriptExData* data = ScriptExData::Get();
	data->imgui += [=]()
	{
		bool spcaing_ = spacing;
		if (count == 1)
		{
			spcaing_ = false;
		}

		float factor = ImGui::GetStyle().ItemSpacing.x / 2.0f;
		float x, y;

		if (count == 3)
		{
			factor = ImGui::GetStyle().ItemSpacing.x / 1.403f;
		}

		if (spcaing_)
		{
			x = ImGui::GetWindowContentRegionWidth() / count - factor;
		}
		else
		{
			x = ImGui::GetWindowContentRegionWidth() / count;
		}

		y = ImGui::GetFrameHeight() * 1.3f;

		data->SetData(buf, 0, x);
		data->SetData(buf, 1, y);
	};

	SetFloatParam(ctx, data->GetData(buf, 0, 10.0f));
	SetFloatParam(ctx, data->GetData(buf, 1, 10.0f));
	return HandlerResult::CONTINUE;
}


void OpcodeMgr::RegisterCommands()
{
	RegisterCommand("IMGUI_BEGIN_FRAME", ImGuiBeginFrame);
	RegisterCommand("IMGUI_END_FRAME", ImGuiEndFrame);

	RegisterCommand("IMGUI_BEGIN", ImGuiBegin);
	RegisterCommand("IMGUI_END", ImGuiEnd);

	RegisterCommand("IMGUI_SET_WINDOW_POS", ImGuiSetWindowPos);
	RegisterCommand("IMGUI_SET_WINDOW_SIZE", ImGuiSetWindowSize);
	RegisterCommand("IMGUI_SET_NEXT_WINDOW_POS", ImGuiSetNextWindowPos);
	RegisterCommand("IMGUI_SET_NEXT_WINDOW_SIZE", ImGuiSetNextWindowSize);
	RegisterCommand("IMGUI_DUMMY", ImGuiDummy);

	RegisterCommand("IMGUI_TEXT", ImGuiText);
	RegisterCommand("IMGUI_TEXT_DISABLED", ImGuiTextDisabled);
	RegisterCommand("IMGUI_TEXT_WRAPPED", ImGuiTextWrapped);
	RegisterCommand("IMGUI_TEXT_COLORED", ImGuiTextColored);
	RegisterCommand("IMGUI_BULLET_TEXT", ImGuiBulletText);
	RegisterCommand("IMGUI_SET_TOOLTIP", ImGuiSetTooltip);

	RegisterCommand("IMGUI_BUTTON", ImGuiButton);
	RegisterCommand("IMGUI_INVISIBLE_BUTTON", ImGuiInvisibleButton);
	RegisterCommand("IMGUI_COLOR_BUTTON", ImGuiColorButton);

	RegisterCommand("IMGUI_CHECKBOX", ImGuiCheckbox);

	RegisterCommand("IMGUI_SAMELINE", ImGuiSameLine);
	RegisterCommand("IMGUI_NEWLINE", ImGuiNewLine);
	RegisterCommand("IMGUI_COLUMNS", ImGuiColumns);
	RegisterCommand("IMGUI_NEXT_COLUMN", ImGuiNextColumn);
	RegisterCommand("IMGUI_SPACING", ImGuiSpacing);
	RegisterCommand("IMGUI_SEPARATOR", ImGuiSeparator);

	RegisterCommand("GET_FRAMERATE", ImGuiGetFramerate);
	RegisterCommand("IMGUI_GET_VERSION", ImGuiGetVersion);
	RegisterCommand("IMGUI_GET_PLUGIN_VERSION", ImGuiGetPluginVersion);

	RegisterCommand("IMGUI_SET_CURSOR_VISIBLE", ImGuiSetCursorVisible);
	RegisterCommand("IMGUI_GET_FRAME_HEIGHT", ImGuiGetFrameHeight);
	RegisterCommand("IMGUI_GET_WINDOW_POS", ImGuiGetWindowPos);
	RegisterCommand("IMGUI_GET_WINDOW_SIZE", ImGuiGetWindowSize);
	RegisterCommand("IMGUI_CALC_TEXT_SIZE", ImGuiCalcTextSize);
	RegisterCommand("IMGUI_GET_WINDOW_CONTENT_REGION_WIDTH", ImGuiGetWindowContentRegionWidth);
	RegisterCommand("IMGUI_BEGIN_MAINMENUBAR", ImGuiBeginMainMenuBar);
	RegisterCommand("IMGUI_END_MAINMENUBAR", ImGuiEndMainMenuBar);
	RegisterCommand("IMGUI_MENU_ITEM", ImGuiMenuItem);
	RegisterCommand("IMGUI_SELECTABLE", ImGuiSelectable);
	RegisterCommand("IMGUI_BEGIN_CHILD", ImGuiBeginChild);
	RegisterCommand("IMGUI_END_CHILD", ImGuiEndChild);
	RegisterCommand("IMGUI_PUSH_ITEM_WIDTH", ImGuiPushItemWidth);
	RegisterCommand("IMGUI_POP_ITEM_WIDTH", ImGuiPopItemWidth);

	RegisterCommand("IMGUI_COLLAPSING_HEADER", ImGuiCollapsingHeader);

	RegisterCommand("IMGUI_SLIDER_INT", ImGuiSliderInt);
	RegisterCommand("IMGUI_SLIDER_FLOAT", ImGuiSliderFloat);
	RegisterCommand("IMGUI_INPUT_INT", ImGuiInputInt);
	RegisterCommand("IMGUI_INPUT_FLOAT", ImGuiInputFloat);
	RegisterCommand("IMGUI_INPUT_TEXT", ImGuiInputText);
	RegisterCommand("IMGUI_RADIO_BUTTON", ImGuiRadioButton);
	RegisterCommand("IMGUI_COLOR_PICKER", ImGuiColorPicker);

	RegisterCommand("IMGUI_IS_ITEM_ACTIVE", ImGuiIsItemActive);
	RegisterCommand("IMGUI_IS_ITEM_CLICKED", ImGuiIsItemClicked);
	RegisterCommand("IMGUI_IS_ITEM_FOCUSED", ImGuiIsItemFocused);
	RegisterCommand("IMGUI_IS_ITEM_HOVERED", ImGuiIsItemHovered);
	RegisterCommand("IMGUI_GET_SCALING_SIZE", ImGuiGetScalingSize);
	RegisterCommand("IMGUI_GET_DISPLAY_SIZE", ImGuiGetDisplaySize);
	RegisterCommand("IMGUI_SET_NEXT_WINDOW_TRANSPARENCY", ImGuiSetNextWindowTransparency);
}
