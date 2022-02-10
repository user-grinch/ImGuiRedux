#include "pch.h"
#include <windows.h>
#include "opcodemgr.h"
#include "d3dhook.h"
#include "depend/injector/injector.hpp"

void ImGuiThread(void* param)
{
    // wait for game init
    Sleep(3000);

	if (D3dHook::InjectHook(&ScriptExData::DrawFrames))
	{
		OpcodeMgr::bImGuiHooked = true;
	}
	else
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
			case GTA3:
			{
				// III 1.0 US
				if (gvm.IsIII() && gvm.IsUS()
					&& gvm.GetMajorVersion() == 1
					&& gvm.GetMinorVersion() == 0
					)
				{
					gGameVer = eGameVer::III;
				}
				break;
			}
			case VC:
			{
				// VC 1.0 US
				if (gvm.IsVC() && gvm.IsUS()
					&& gvm.GetMajorVersion() == 1
					&& gvm.GetMinorVersion() == 0
					)
				{
					gGameVer = eGameVer::VC;
				}
				break;
			}
			case SA:
			{
				// SA US 1.0 Hoodlum
				if (gvm.IsHoodlum())
				{
					gGameVer = eGameVer::SA;
				}
				break;
			}
			case GTA3_UNREAL:
			{
				gGameVer = eGameVer::III_DE;
				break;
			}
			case VC_UNREAL:
			{
				gGameVer = eGameVer::VC_DE;
				break;
			}
			case SA_UNREAL:
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

		if (gGameVer == eGameVer::Unknown)
		{
			Log("[ImGuiRedux] Unsupported game/version!");
			MessageBox(HWND_DESKTOP, "Unsupported game/version!", "ImGuiRedux", MB_ICONERROR);
		}
		else
		{
			OpcodeMgr::RegisterCommands();
			CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&ImGuiThread, nullptr, NULL, nullptr);
		}
    }

    return TRUE;
}
