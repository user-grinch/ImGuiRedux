#include "pch.h"
#include <windows.h>
#include "version.h"
#include "opcode_mgr.h"
#include "d3dhook.h"

static void TestWindow()
{
	D3dHook::SetMouseState(true);
	ImGui::Begin("TEST");
	ImGui::Text("ImGuiRedux");
	ImGui::End();
}

class ImGuiRedux {
public:
	ImGuiRedux() 
	{
		Log("Init ImGuiRedux");

		uint gameVer = GetGameVersion();
		if (gameVer != GAME_10US_HOODLUM && gameVer != GAME_10US_COMPACT)
		{
			Log("[ImGuiRedux] Unsupported game. Only GTA SA Classic is supported.");
			MessageBox(HWND_DESKTOP, "Unsupported game. Only GTA SA Classic is supported.", "ImGuiRedux", MB_ICONERROR);
			return;
		}

		Events::initGameEvent += []()
		{
			if (D3dHook::InjectHook(&TestWindow))
			{
				OpcodeMgr::bImGuiHooked = true;
			}
			else
			{
				Log("[ImGuiRedux] Failed to inject dxhook.");
				MessageBox(HWND_DESKTOP, "Failed to inject dxhook..", "ImGuiRedux", MB_ICONERROR);
			}
		};

		OpcodeMgr::RegisterCommands();
	}
} ImGuiRedux_;
