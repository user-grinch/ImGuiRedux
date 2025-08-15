#include "pch.h"
#include "opcodemgr.h"
#include "hook.h"
#include "wrapper.hpp"

#include <psapi.h>
#include <windows.h>

HINSTANCE gDllHandle;

DWORD GetProcessIdFromModule(HMODULE hModule) {
    DWORD processId = 0;
    HANDLE hProcess = NULL;
    MODULEINFO moduleInfo;
    
    hProcess = GetCurrentProcess();
    if (hProcess != NULL) {
        if (GetModuleInformation(hProcess, hModule, &moduleInfo, sizeof(moduleInfo))) {
            processId = GetProcessId(hProcess);
        }
        CloseHandle(hProcess);
    }

    return processId;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processId = 0;
    GetWindowThreadProcessId(hwnd, &processId);

    if (processId == static_cast<DWORD>(lParam)) {
        return FALSE; // Stop
    }
    return TRUE; // Continue
}

bool HasGameLaunched(HMODULE hModule, int maxRetries, int sleepDuration) {
    DWORD processId = GetProcessIdFromModule(hModule);
    if (processId == 0) {
        MessageBox(NULL, "Failed to get process ID", "ImGuiRedux", MB_ICONERROR);
        return false;
    }

    int retries = 0;
    while (retries < maxRetries) {
        Sleep(sleepDuration);
        if (!EnumWindows(EnumWindowsProc, static_cast<LPARAM>(processId))) {
            return true;
        }
        retries++;
    }
    MessageBox(NULL, "Failed to detect game window.", "ImGuiRedux", MB_ICONERROR);
    return false;
}

void ImGuiThread(void* param) {
    if (!HasGameLaunched(gDllHandle, 30, 5000)) {
        return;
    }

    if (gGameVer <= eGameVer::SA) {
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

    if (!Hook::Inject(&ScriptExData::RenderFrames)) {
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
        CLEO::eGameVersion ver = CLEO::CLEO_GetGameVersion();
        if (ver != CLEO::eGameVersion::GV_US10) {
            MessageBox(HWND_DESKTOP, "Unknown game/ version detected. gta_sa.exe v1.0 US required!", "ImGuiRedux", MB_ICONERROR);
        }
        gGameVer = eGameVer::SA;
#else   
        if (GetModuleHandle("ImGuiCleoWin32.cleo")) {
            wLog("ImGuiReduxWin32: ImGuiCleoWin32 detected. Closing...");
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
