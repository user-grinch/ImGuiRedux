#include "pch.h"
#include "hook.h"
#include "kiero.h"
#include "MinHook.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_win32.h"
#include "injector.hpp"
#include "font.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static const ImWchar* GetGlyphRanges()
{
    static const ImWchar ranges[] =
    {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0980, 0x09FF, // Bengali
        0x2000, 0x206F, // General Punctuation

        // Chinease
        // 0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        // 0x31F0, 0x31FF, // Katakana Phonetic Extensions
        // 0xFF00, 0xFFEF, // Half-width characters
        // 0xFFFD, 0xFFFD, // Invalid
        // 0x4E00, 0x9FAF, // CJK Ideograms

        // Russian
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B
        0,
    };
    return &ranges[0];
}

bool Hook::GetMouseState()
{
    return mouseShown;
}

void Hook::SetMouseState(bool state)
{
    mouseShown = state;
}

LRESULT Hook::hkWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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

HRESULT Hook::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    return oReset(pDevice, pPresentationParameters);
}

void Hook::ProcessFrame(void* ptr)
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
        static int height, width, RsGlobal;
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
            RECT rect;
            GetWindowRect(hwnd, &rect);
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
        
        static ImVec2 fScreenSize = ImVec2(-1, -1);
        if (fScreenSize.x != width && fScreenSize.y != height)
        {
            if (gRenderer == eRenderer::Dx9)
            {
                ImGui_ImplDX9_InvalidateDeviceObjects();
            }
            else if (gRenderer == eRenderer::Dx11)
            {
                ImGui_ImplDX11_InvalidateDeviceObjects();
            }
            
            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();
            float fontSize = height / 50.0f;
            io.FontDefault = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fontData, fontSize, NULL, GetGlyphRanges());
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
        if (gRenderer == eRenderer::Dx9)
        {
            ImGui_ImplDX9_NewFrame();
        }
        else if (gRenderer == eRenderer::Dx11)
        {
            ImGui_ImplDX11_NewFrame();
        }
        else
        {
            ImGui_ImplOpenGL3_NewFrame();
        }

        ImGui::NewFrame();

        if (pCallbackFunc != nullptr)
        {
            ((void(*)())pCallbackFunc)();
        }

        ImGui::EndFrame();
        ImGui::Render();

        if (gRenderer == eRenderer::Dx9)
        {
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        }
        else if (gRenderer == eRenderer::Dx11)
        {
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        }
        else
        {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    }
    else
    {
        if (!ImGui_ImplWin32_Init(hwnd))
        {
            return;
        }

        if (gGameVer == eGameVer::SA)
        {
            injector::MakeNOP(0x00531155, 5); // shift trigger fix
        }

        if (gRenderer == eRenderer::Dx9)
        {
            if (!ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(ptr)))
            {
                return;
            }
        }
        else if (gRenderer == eRenderer::Dx11)
        {
            // for dx11 device ptr is swapchain
            reinterpret_cast<IDXGISwapChain*>(ptr)->GetDevice(__uuidof(ID3D11Device), &ptr);
            ID3D11DeviceContext* context;
            reinterpret_cast<ID3D11Device*>(ptr)->GetImmediateContext(&context);

            if(!ImGui_ImplDX11_Init(reinterpret_cast<ID3D11Device*>(ptr), context))
            {
                return;
            }
        }
        else
        {
            if(!ImGui_ImplOpenGL3_Init())
            {
                return;
            }
        }

        ImGui_ImplWin32_EnableDpiAwareness();

        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#ifdef _WIN64
        oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LRESULT)hkWndProc);
#else
        oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWL_WNDPROC, (LRESULT)hkWndProc);
#endif
        init = true;
    }
}

HRESULT Hook::hkEndScene(IDirect3DDevice9* pDevice)
{
    ProcessFrame(pDevice);
    return oEndScene(pDevice);
}

HRESULT Hook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    ProcessFrame(pSwapChain);
    return oPresent(pSwapChain, SyncInterval, Flags);
}

bool Hook::hkGlSwapBuffer(_In_ HDC hDc)
{
    ProcessFrame(nullptr);
    return oGlSwapBuffer(hDc);
}

void Hook::ProcessMouse()
{
    static bool curState;
    if (curState != mouseShown)
    {
        ImGui::GetIO().MouseDrawCursor = mouseShown;

        /* 
            Skip mouse patches on unknown host
            ImGui menus should be interactive on game menu
        */
        if (gGameVer != eGameVer::Unknown)
        {
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
        }

        curState = mouseShown;
    }
}

bool Hook::Inject(void *pCallback)
{
    static bool injected;
    if (injected)
    {
        return false;
    }

    ImGui::CreateContext();

    /*
        Must check for d3d9 first!
        Seems to crash with nvidia geforce experience overlay
        if anything else is checked before d3d9
    */
    hwnd = GetForegroundWindow();
    if (GetModuleHandle("_gtaRenderHook.asi"))
    {
        goto dx11;
    }
    
    if (init(kiero::RenderType::D3D9) == kiero::Status::Success)
    {
        gRenderer = eRenderer::Dx9;
        injected = true;
        kiero::bind(16, (void**)&oReset, hkReset);
        kiero::bind(42, (void**)&oEndScene, hkEndScene);
        pCallbackFunc = pCallback;
    }

    if (init(kiero::RenderType::OpenGL) == kiero::Status::Success)
    {
        gRenderer = eRenderer::OpenGL;
        injected = true;

        HMODULE hMod = GetModuleHandle("OPENGL32.dll");
        if (!hMod)
        {
            return false;
        }
        FARPROC addr = GetProcAddress(hMod, "wglSwapBuffers");
        MH_Initialize();
        MH_CreateHook(addr, hkGlSwapBuffer, (void**)&oGlSwapBuffer);
        MH_EnableHook(addr);
        pCallbackFunc = pCallback;
    }
    
    dx11:
    if (init(kiero::RenderType::D3D11) == kiero::Status::Success)
    {
        gRenderer = eRenderer::Dx11;
        kiero::bind(8, (void**)&oPresent, hkPresent);
        pCallbackFunc = pCallback;
        injected = true;
    }

    return injected;
}

void Hook::Remove()
{
    pCallbackFunc = nullptr;
#ifdef _WIN64
    SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LRESULT)oWndProc);     
#else
    SetWindowLongPtr(hwnd, GWL_WNDPROC, (LRESULT)oWndProc);
#endif

    if (gRenderer == eRenderer::Dx9)
    {
        ImGui_ImplDX9_Shutdown();
    }
    if (gRenderer == eRenderer::Dx11)
    {
        ImGui_ImplDX11_Shutdown();
    }
    else
    {
        ImGui_ImplOpenGL3_Shutdown();
    }

    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    kiero::shutdown();
}