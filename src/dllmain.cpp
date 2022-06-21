#include "pch.h"
#include <windows.h>
#include "opcodemgr.h"
#include "hook.h"
#include "injector.hpp"

void ImGuiThread(void* param)
{
    // wait for game init
    Sleep(3000);

	/*
		Need SP for mouse fixes
		Only need for classics
		TODO: Get the mouse patches from MTA later
	*/
	if (gGameVer <= eGameVer::SA)
	{
		std::string moduleName = "SilentPatchSA.asi";
		if (gGameVer == eGameVer::VC)
		{
			moduleName = "SilentPatchVC.asi";
		}
		else if (gGameVer == eGameVer::III)
		{
			moduleName = "SilentPatchIII.asi";
		}

		if (!GetModuleHandle(moduleName.c_str()))
		{
			Log("[ImGuiRedux] SilentPatch not found. Please install it from here https://gtaforums.com/topic/669045-silentpatch/");
			int msgID = MessageBox(NULL, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", "ImGuiRedux", MB_OKCANCEL | MB_DEFBUTTON1);

			if (msgID == IDOK)
			{
				ShellExecute(nullptr, "open", "https://gtaforums.com/topic/669045-silentpatch/", nullptr, nullptr, SW_SHOWNORMAL);
			};
			return;
		}
	}

	if (!Hook::Inject(&ScriptExData::DrawFrames))
	{
		Log("[ImGuiRedux] Failed to inject dxhook.");
		MessageBox(HWND_DESKTOP, "Failed to inject dxhook..", "ImGuiRedux", MB_ICONERROR);
	}

    while (true)
    {
        Sleep(5000);
    }
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
    if (nReason == DLL_PROCESS_ATTACH)
    {
		HostId id = GetHostId();
		auto gvm = injector::game_version_manager();
		gvm.Detect();

		switch (id)
		{
			case HostId::GTA3:
			{
				// III 1.0 US
				if (gvm.IsIII()
					&& gvm.GetMajorVersion() == 1
					&& gvm.GetMinorVersion() == 0
					)
				{
					gGameVer = eGameVer::III;
				}
				break;
			}
			case HostId::VC:
			{
				// VC 1.0 US
				if (gvm.IsVC() 
					&& gvm.GetMajorVersion() == 1
					&& gvm.GetMinorVersion() == 0
					)
				{
					gGameVer = eGameVer::VC;
				}
				break;
			}
			case HostId::SA:
			{
				// SA US 1.0
				if (gvm.IsSA()
				&& gvm.GetMajorVersion() == 1
				&& gvm.GetMinorVersion() == 0
					)
				{
					gGameVer = eGameVer::SA;
				}
				break;
			}
			case HostId::GTA3_UNREAL:
			{
				gGameVer = eGameVer::III_DE;
				break;
			}
			case HostId::VC_UNREAL:
			{
				gGameVer = eGameVer::VC_DE;
				break;
			}
			case HostId::SA_UNREAL:
			{
				gGameVer = eGameVer::SA_DE;
				break;
			}
			default:
			{
				gGameVer = eGameVer::Unknown;
				break;
			}
		}

		OpcodeMgr::RegisterCommands();
		CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&ImGuiThread, nullptr, NULL, nullptr);
    }

    return TRUE;
}
