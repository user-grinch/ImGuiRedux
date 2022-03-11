#include "pch.h"
#include "d3dhook.h"
#include "kiero/kiero.h"
#include "kiero/minhook/MinHook.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "injector/injector.hpp"

eRenderer gRenderer;
eGameVer gGameVer;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool D3dHook::GetMouseState()
{
    return mouseShown;
}

void D3dHook::SetMouseState(bool state)
{
    mouseShown = state;
}

LRESULT D3dHook::hkWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

    if (ImGui::GetIO().WantTextInput)
    {
        if (gGameVer == eGameVer::SA)
        {
            reinterpret_cast<void(__cdecl*)()>(0x53F1E0)(); // CPad::ClearKeyboardHistory
        }
        return 1;
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT D3dHook::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    return oReset(pDevice, pPresentationParameters);
}

void D3dHook::ProcessFrame(void* ptr)
{
    if (!ImGui::GetCurrentContext())
    {
        ImGui::CreateContext();
    }

    ImGuiIO& io = ImGui::GetIO();
    static bool init;
    if (init)
    {
        ProcessMouse();

        // Scale the menu if game resolution changed
        int height, width, RsGlobal;
        if (gGameVer == eGameVer::III)
        {
            RsGlobal = 0x8F4360;
            width = injector::ReadMemory<int>(RsGlobal + 4, 0);      // width
            height = injector::ReadMemory<int>(RsGlobal + 8, 0);    // height
        }
        else if (gGameVer == eGameVer::VC)
        {
            RsGlobal = 0x9B48D8;
            width = injector::ReadMemory<int>(RsGlobal + 4, 0);      // width
            height = injector::ReadMemory<int>(RsGlobal + 8, 0);    // height
        }
        else if (gGameVer == eGameVer::SA)
        {
            RsGlobal = 0xC17040;
            width = injector::ReadMemory<int>(RsGlobal + 4, 0);      // width
            height = injector::ReadMemory<int>(RsGlobal + 8, 0);    // height
        }   
        else
        {
            // TODO DE
            // This was crashing during window tab+alt
            // probably need to replace this later

            RECT rect;
            GetWindowRect(GetForegroundWindow(), &rect);
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
        
        static ImVec2 fScreenSize = ImVec2(-1, -1);
        if (fScreenSize.x != width && fScreenSize.y != height)
        {
            if (gRenderer == eRenderer::DX9)
            {
                ImGui_ImplDX9_InvalidateDeviceObjects();
            }
            else
            {
                ImGui_ImplDX11_InvalidateDeviceObjects();
            }
            
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();
            float fontSize = height / 54.85f;
            io.FontDefault = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/trebucbd.ttf", fontSize);
            io.Fonts->Build();

            ImGuiStyle* style = &ImGui::GetStyle();
            float scaleX = width / 1366.0f;
            float scaleY = height / 768.0f;

            style->ChildBorderSize = 0;
            style->WindowBorderSize = 0;
            style->FrameBorderSize = 0;
            style->TabBorderSize = 0;
            style->PopupBorderSize = 0;
            style->FramePadding = ImVec2(5 * scaleX, 5 * scaleY);
            style->ItemSpacing = ImVec2(8 * scaleX, 4 * scaleY);
            style->ScrollbarSize = 12 * scaleX;
            style->IndentSpacing = 20 * scaleX;
            style->ItemInnerSpacing = ImVec2(5 * scaleX, 5 * scaleY);
            style->Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
            style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
            style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
            fScreenSize = ImVec2((float)width, (float)height);
        }

        ImGui_ImplWin32_NewFrame();
        if (gRenderer == eRenderer::DX9)
        {
            ImGui_ImplDX9_NewFrame();
        }
        else
        {
            ImGui_ImplDX11_NewFrame();
        }

        ImGui::NewFrame();

        if (pCallbackFunc != nullptr)
        {
            ((void(*)())pCallbackFunc)();
        }

        ImGui::EndFrame();
        ImGui::Render();

        if (gRenderer == eRenderer::DX9)
        {
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        }
        else
        {
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
    }
    else
    {
        init = true;
        ImGui_ImplWin32_Init(GetForegroundWindow());

        if (gGameVer == eGameVer::SA)
        {
            injector::MakeNOP(0x00531155, 5); // shift trigger fix
        }

        if (gRenderer == eRenderer::DX9)
        {
            ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(ptr));
        }
        else
        {
            // for dx11 device ptr is swapchain
            reinterpret_cast<IDXGISwapChain*>(ptr)->GetDevice(__uuidof(ID3D11Device), &ptr);
            ID3D11DeviceContext* context;
            reinterpret_cast<ID3D11Device*>(ptr)->GetImmediateContext(&context);

            ImGui_ImplDX11_Init(reinterpret_cast<ID3D11Device*>(ptr), context);
        }

        ImGui_ImplWin32_EnableDpiAwareness();

        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        oWndProc = (WNDPROC)SetWindowLongPtr(GetForegroundWindow(), GWL_WNDPROC, (LRESULT)hkWndProc);
    }
}

HRESULT D3dHook::hkEndScene(IDirect3DDevice9* pDevice)
{
    ProcessFrame(pDevice);
    return oEndScene(pDevice);
}

HRESULT D3dHook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    ProcessFrame(pSwapChain);
    return oPresent(pSwapChain, SyncInterval, Flags);
}

void D3dHook::ProcessMouse()
{
    static bool mouseState;
    
    if (mouseState != mouseShown)
    {
        ImGui::GetIO().MouseDrawCursor = mouseShown;

        if (gGameVer == eGameVer::SA)
        {
            if (mouseShown)
            {
                injector::WriteMemory<unsigned char>(0x6194A0, 0xC3);
                injector::MakeNOP(0x541DD7, 5);
            }
            else
            {
                injector::WriteMemory<unsigned char>(0x6194A0, 0xE9);
                injector::WriteMemoryRaw(0x541DD7, (char*)"\xE8\xE4\xD5\xFF\xFF", 5, false);
            }

            // ClearMouseStates
            injector::WriteMemory<float>(0xB73418 + 12, 0); // X
            injector::WriteMemory<float>(0xB73418 + 16, 0); // Y

            reinterpret_cast<void(__cdecl*)()>(0x541BD0)(); // CPad::ClearMouseHistory();
            reinterpret_cast<void(__cdecl*)()>(0x541DD0)(); // CPad::UpdatePads();
        } 
        else if (gGameVer == eGameVer::VC)
        {
            if (mouseShown)
            {
                injector::WriteMemory<unsigned char>(0x6020A0, 0xC3, true);
                injector::MakeNOP(0x4AB6CA, 5, true);
            }
            else
            {
                injector::WriteMemory<unsigned char>(0x6020A0, 0x53, true);
                injector::WriteMemoryRaw(0x4AB6CA, (char*)"\xE8\x51\x21\x00\x00", 5, true);
            }

            // ClearMouseStates
            injector::WriteMemory<float>(0x94D788 + 8, 0); // X
            injector::WriteMemory<float>(0x94D788 + 12, 0);// Y

            reinterpret_cast<void(__cdecl*)()>(0x4ADB30)(); // CPad::ClearMouseHistory();
            reinterpret_cast<void(__cdecl*)()>(0x4AB6C0)(); // CPad::UpdatePads();
        }
        else if (gGameVer == eGameVer::III)
        {
            if (mouseShown)
            {
                injector::WriteMemory<unsigned char>(0x580D20, 0xC3, true);
                injector::MakeNOP(0x49272F, 5, true);
            }
            else
            {
                injector::WriteMemory<unsigned char>(0x580D20, 0x53, true);
                injector::WriteMemoryRaw(0x49272F, (char*)"\xE8\x6C\xF5\xFF\xFF", 5, true);
            }

            // ClearMouseStates
            injector::WriteMemory<float>(0x8809F0 + 8, 0); // X
            injector::WriteMemory<float>(0x8809F0 + 12, 0);// Y
            
            int pad = reinterpret_cast<int(__thiscall*)(int)>(0x492F60)(NULL); // CPad::GetPads();
            reinterpret_cast<void(__thiscall*)(int)>(0x491B50)(pad); // CPad::ClearMouseHistory();
            reinterpret_cast<void(__cdecl*)()>(0x492720)(); // CPad::UpdatePads();
        }
        // TODO DE 

        mouseState = mouseShown;
    }
}

bool D3dHook::InjectHook(void *pCallback)
{
    static bool hookInjected;
    if (hookInjected)
    {
        return false;
    }

    ImGui::CreateContext();

    /*
        Must check for d3d9 first!
        Seems to crash with nvidia geforce experience overlay
        if anything else is checked before d3d9
    */
    if (init(kiero::RenderType::D3D9) == kiero::Status::Success)
    {
        gRenderer = eRenderer::DX9;
        hookInjected = true;
        kiero::bind(16, (void**)&oReset, hkReset);
        kiero::bind(42, (void**)&oEndScene, hkEndScene);
        pCallbackFunc = pCallback;
    }
    else
    {

        if (init(kiero::RenderType::D3D11) == kiero::Status::Success)
        {
            gRenderer = eRenderer::DX11;
            kiero::bind(8, (void**)&oPresent, hkPresent);
            pCallbackFunc = pCallback;
            hookInjected = true;
        }
        else
        {
            gRenderer = eRenderer::UNKNOWN;
        }
    }

    return hookInjected;
}

void D3dHook::RemoveHook()
{
    pCallbackFunc = nullptr;
    SetWindowLongPtr(GetForegroundWindow(), GWL_WNDPROC, (LRESULT)oWndProc);
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    kiero::shutdown();
}