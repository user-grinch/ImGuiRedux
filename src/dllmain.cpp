#include "pch.h"
#include <windows.h>
#include "opcodemgr.h"
#include "hook.h"
#include "injector.hpp"
#include "MinHook.h"

void f_GTA_SPCheck() {
    std::string moduleName = "SilentPatchSA.asi";
    if (gGameVer == eGameVer::VC) {
        moduleName = "SilentPatchVC.asi";
    } else if (gGameVer == eGameVer::III) {
        moduleName = "SilentPatchIII.asi";
    }

    if (!GetModuleHandle(moduleName.c_str())) {
        int msgID = MessageBox(NULL, "SilentPatch not found. Do you want to install Silent Patch? (Game restart required)", "ImGuiRedux", MB_OKCANCEL | MB_DEFBUTTON1);
        if (msgID == IDOK) {
            ShellExecute(nullptr, "open", "https://gtaforums.com/topic/669045-silentpatch/", nullptr, nullptr, SW_SHOWNORMAL);
        };
        return;
    }
}

void ImGuiThread(void* param) {
    /*
    	Need SP for mouse fixes
    	Only need for classics
    	TODO: Get the mouse patches from MTA later
    */
    if (gGameVer <= eGameVer::SA) {
        MH_Initialize();
        uint32_t addr = (gGameVer == eGameVer::SA) ? 0x5BF3A1 :
                        ((gGameVer == eGameVer::VC) ? 0x4A5B6B : 0x48D52F);

        void *ptr = NULL;
        MH_CreateHook((void*)addr, f_GTA_SPCheck, &ptr);
        MH_EnableHook(ptr);
    }

    Sleep(5000);

    if (!Hook::Inject(&ScriptExData::DrawFrames)) {
        MessageBox(HWND_DESKTOP, "Failed to inject dxhook..", "ImGuiRedux", MB_ICONERROR);
    }

    while (true) {
        Sleep(5000);
    }
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved) {
    if (nReason == DLL_PROCESS_ATTACH) {
        auto gvm = injector::game_version_manager();
        gvm.Detect();

        if (gvm.GetMajorVersion() == 1 && gvm.GetMinorVersion() == 0) {
            if (gvm.IsIII()) {
                gGameVer = eGameVer::III;
            }

            if (gvm.IsVC()) {
                gGameVer = eGameVer::VC;
            }

            if (gvm.IsSA()) {
                gGameVer = eGameVer::SA;
            }
        }

        if constexpr (gRunTime == eRunTime::Redux) {
            HostId id = GetHostId();
            switch (id) {
                case HostId::GTA3_UNREAL: {
                    gGameVer = eGameVer::III_DE;
                    break;
                }
                case HostId::VC_UNREAL: {
                    gGameVer = eGameVer::VC_DE;
                    break;
                }
                case HostId::SA_UNREAL: {
                    gGameVer = eGameVer::SA_DE;
                    break;
                }
                case HostId::BULLY: {
                    gGameVer = eGameVer::BullySE;
                    break;
                }
            }
        } 

        OpcodeMgr::RegisterCommands();
        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&ImGuiThread, nullptr, NULL, nullptr);
    }

    return TRUE;
}
