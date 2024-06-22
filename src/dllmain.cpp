#include "pch.h"
#include <windows.h>
#include "opcodemgr.h"
#include "hook.h"
#include "wrapper.hpp"

#ifdef RUNTIME_CLEO
#include "MinHook.h"
#include "injector.hpp"

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
#endif

void ImGuiThread(void* param) {

#ifdef RUNTIME_CLEO
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
#endif

    Sleep(5000);

    if (!Hook::Inject(&ScriptExData::DrawFrames)) {
        MessageBox(HWND_DESKTOP, "Failed to inject dxhook..", "ImGuiRedux", MB_ICONERROR);
    }

    while (true) {
        Sleep(5000);
    }
}

void __stdcall _wrapper(DWORD saveSlot) {
    ScriptExData::Clear();
}

BOOL WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved) {
    if (nReason == DLL_PROCESS_ATTACH) {
#ifdef RUNTIME_CLEO
        auto gvm = injector::game_version_manager();
        gvm.Detect();
        
        if (gvm.GetMajorVersion() == 1 && gvm.GetMinorVersion() == 0) {
            if (gvm.IsIII()) gGameVer = eGameVer::III;
            if (gvm.IsVC()) gGameVer = eGameVer::VC;
            if (gvm.IsSA()) gGameVer = eGameVer::SA;
        }
#else   
        if (GetModuleHandle("ImGuiCleoWin32.cleo")) {
            Log("ImGuiReduxWin32: ImGuiCleoWin32 detected. Closing...");
            return TRUE;
        }

        auto id = GetHostId();
        if (id == HostId::GTA3) gGameVer = eGameVer::III;
        if (id == HostId::VC) gGameVer = eGameVer::VC;
        if (id == HostId::SA) gGameVer = eGameVer::SA;
        if (id == HostId::IV) gGameVer = eGameVer::IV;
        if (id == HostId::GTA3_UNREAL) gGameVer = eGameVer::III_DE;
        if (id == HostId::VC_UNREAL) gGameVer = eGameVer::VC_DE;
        if (id == HostId::SA_UNREAL) gGameVer = eGameVer::SA_DE;
        if (id == HostId::BULLY) gGameVer = eGameVer::BullySE;
#endif

#ifdef RUNTIME_CLEO
        CLEO::CLEO_RegisterCallback(CLEO::eCallbackId::GameBegin, _wrapper);
#else
        OnRuntimeInit((OnRuntimeInitCallback)ScriptExData::Clear);
#endif

        OpcodeMgr::RegisterCommands();
        CreateThread(nullptr, NULL, (LPTHREAD_START_ROUTINE)&ImGuiThread, nullptr, NULL, nullptr);
    }

    return TRUE;
}
