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
#include "kiero.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static const ImWchar* GetGlyphRanges() {
    static const ImWchar ranges[] = {
        0x0020, 0x00FF, // Basic Latin + Latin Supplement
        0x0980, 0x09FF, // Bengali
        0x2000, 0x206F, // General Punctuation

        // Chinease
        // 0x3000, 0x30FF, // CJK Symbols and Punctuations, Hiragana, Katakana
        // 0x31F0, 0x31FF, // Katakana Phonetic Extensions
        // 0xFF00, 0xFFEF, // Half-width characters
        // 0xFFFD, 0xFFFD, // Invalid
        // 0x4E00, 0x9FAF, // CJK Ideograms

        // Cyrillic
        0x0400, 0x052F, // Cyrillic + Cyrillic Supplement
        0x2DE0, 0x2DFF, // Cyrillic Extended-A
        0xA640, 0xA69F, // Cyrillic Extended-B

        //Turkish
        0x011E, 0x011F,
        0x015E, 0x015F,
        0x0130, 0x0131,
        0,
    };
    return &ranges[0];
}

bool Hook::GetMouseState() {
    return mouseVisible;
}

void Hook::SetMouseState(bool state) {
    mouseVisible = state;
}

LRESULT Hook::hkWndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);

    if (ImGui::GetIO().WantTextInput || (gGameVer > eGameVer::SA && mouseVisible)) {
#ifndef _WIN64
        if (gGameVer == eGameVer::SA) {
            reinterpret_cast<void(__cdecl*)()>(0x53F1E0)(); // CPad::ClearKeyboardHistory
        }
#endif
        return 1;
    }

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

HRESULT Hook::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters) {
    ImGui_ImplDX9_InvalidateDeviceObjects();

    return oReset(pDevice, pPresentationParameters);
}

void Hook::ProcessFrame(void* ptr) {
    static bool init;
    if (init) {
        ProcessMouse();

        // Scale the menu if game resolution changed
        static int height, width, RsGlobal;
#ifdef RUNTIME_CLEO
        if (gGameVer == eGameVer::III) {
            RsGlobal = 0x8F4360;
            width = injector::ReadMemory<int>(RsGlobal + 4, 0);      // width
            height = injector::ReadMemory<int>(RsGlobal + 8, 0);    // height
        } else if (gGameVer == eGameVer::VC) {
            RsGlobal = 0x9B48D8;
            width = injector::ReadMemory<int>(RsGlobal + 4, 0);      // width
            height = injector::ReadMemory<int>(RsGlobal + 8, 0);    // height
        } else if (gGameVer == eGameVer::SA) {
            RsGlobal = 0xC17040;
            width = injector::ReadMemory<int>(RsGlobal + 4, 0);      // width
            height = injector::ReadMemory<int>(RsGlobal + 8, 0);    // height
        } else {
            RECT rect;
            GetWindowRect(hwnd, &rect);
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
        }
#else
        RECT rect;
        GetWindowRect(hwnd, &rect);
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
#endif
        static ImVec2 fScreenSize = ImVec2(-1, -1);
        if (fScreenSize.x != width && fScreenSize.y != height) {
            if (gRenderer == eRenderer::Dx9) {
                ImGui_ImplDX9_InvalidateDeviceObjects();
            } else if (gRenderer == eRenderer::Dx11) {
                ImGui_ImplDX11_InvalidateDeviceObjects();
            }

            ImGuiIO& io = ImGui::GetIO();
            io.Fonts->Clear();
            float fontSize = height / 48.0f;
            io.FontDefault = io.Fonts->AddFontFromMemoryCompressedBase85TTF(fontData, fontSize, NULL, GetGlyphRanges());
            io.Fonts->Build();

            ImGuiStyle* style = &ImGui::GetStyle();
            float scaleX = width / 1366.0f;
            float scaleY = height / 768.0f;

            style->TabRounding = 0.0f;
            style->ChildBorderSize = 0;
            style->WindowBorderSize = 0;
            style->FrameBorderSize = 0;
            style->TabBorderSize = 0;
            style->PopupBorderSize = 0;
            style->FramePadding = ImVec2(5 * scaleX, 3 * scaleY);
            style->ItemSpacing = ImVec2(4 * scaleY, 4 * scaleY);
            style->ScrollbarSize = 12 * scaleX;
            style->IndentSpacing = 20 * scaleX;
            style->ItemInnerSpacing = ImVec2(4 * scaleY, 4 * scaleY);
            style->Colors[ImGuiCol_Header] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
            style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.0f, 0.0f, 0.0f, 0.00f);
            style->WindowTitleAlign = ImVec2(0.5f, 0.5f);
            fScreenSize = ImVec2((float)width, (float)height);
        }

        ImGui_ImplWin32_NewFrame();
        if (gRenderer == eRenderer::Dx9) {
            ImGui_ImplDX9_NewFrame();
        } else if (gRenderer == eRenderer::Dx11) {
            ImGui_ImplDX11_NewFrame();
        } else {
            ImGui_ImplOpenGL3_NewFrame();
        }

        ImGui::NewFrame();

        if (pCallbackFunc != nullptr) {
            static_cast<void(*)()>(pCallbackFunc)();
        }

        ImGui::EndFrame();
        ImGui::Render();

        if (gRenderer == eRenderer::Dx9) {
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        } else if (gRenderer == eRenderer::Dx11) {
            pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);
            ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        } else {
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
    } else {
        if (!ImGui::GetCurrentContext()) {
            ImGui::CreateContext();
        }

        if (gGameVer == eGameVer::SA) {
            injector::MakeNOP(0x00531155, 5); // shift trigger fix
        }

        if (gRenderer == eRenderer::Dx9) {
            hwnd = GetForegroundWindow();
            if (!ImGui_ImplWin32_Init(hwnd)) {
                return;
            }

            if (!ImGui_ImplDX9_Init(reinterpret_cast<IDirect3DDevice9*>(ptr))) {
                return;
            }
            gD3DDevice = ptr;
        } else if (gRenderer == eRenderer::Dx11) {
            IDXGISwapChain* pSwapChain = reinterpret_cast<IDXGISwapChain*>(ptr);
            if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), &ptr))) {
                ID3D11Device *pDevice = reinterpret_cast<ID3D11Device*>(ptr);
                pDevice->GetImmediateContext(&pDeviceContext);

                DXGI_SWAP_CHAIN_DESC Desc;
                pSwapChain->GetDesc(&Desc);
                hwnd = Desc.OutputWindow;

                ID3D11Texture2D* backBuffer;
                pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
                pDevice->CreateRenderTargetView(backBuffer, NULL, &pRenderTargetView);
                backBuffer->Release();

                if (!ImGui_ImplWin32_Init(hwnd)) {
                    return;
                }
                ImGui_ImplDX11_Init(pDevice, pDeviceContext);
                ImGui_ImplDX11_CreateDeviceObjects();
                ImGui::GetIO().ImeWindowHandle = hwnd;
            }

            gD3DDevice = ptr;
        } else {
            hwnd = GetForegroundWindow();
            if (!ImGui_ImplWin32_Init(hwnd)) {
                return;
            }

            if(!ImGui_ImplOpenGL3_Init()) {
                return;
            }
        }

        ImGui_ImplWin32_EnableDpiAwareness();

        ShowCursor(false);
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;
        io.LogFilename = nullptr;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad | ImGuiConfigFlags_NoMouseCursorChange;

        oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, -4, (LRESULT)hkWndProc); // GWL_WNDPROC = -4
        init = true;
    }
}

HRESULT Hook::hkEndScene(IDirect3DDevice9* pDevice) {
    ProcessFrame(pDevice);
    return oEndScene(pDevice);
}

HRESULT Hook::hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    ProcessFrame(pSwapChain);
    return oPresent(pSwapChain, SyncInterval, Flags);
}

HRESULT Hook::hkResizeBuffers(IDXGISwapChain* pSwapChain, UINT a, UINT b, UINT c, DXGI_FORMAT d, UINT e) {
    if (pRenderTargetView) {
        pRenderTargetView->Release();
        pRenderTargetView = nullptr;
        pDeviceContext->Flush();
    }

    HRESULT hr = oResizeBuffers(pSwapChain, a, b, c, d, e);
    ID3D11Texture2D* back_buffer{};
    pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&back_buffer);
    reinterpret_cast<ID3D11Device*>(pSwapChain)->CreateRenderTargetView(back_buffer, nullptr, &pRenderTargetView);
    back_buffer->Release();
    return hr;
}

bool Hook::hkGlSwapBuffer(HDC unnamedParam1, UINT unnamedParam2) {
    ProcessFrame(nullptr);
    return oGlSwapBuffer(unnamedParam1, unnamedParam2);
}

void Hook::ProcessMouse() {
    static bool curState = false;
    if (curState != mouseVisible) {
        ImGui::GetIO().MouseDrawCursor = mouseVisible;

        /*
            Skip mouse patches on unknown host
            ImGui menus should be interactive on game menu
        */
#ifndef _WIN64
        if (gGameVer == eGameVer::SA) {
            if (ImGui::GetIO().MouseDrawCursor) {
                injector::WriteMemory<unsigned char>(0x6194A0, 0xC3, true);
                injector::MakeNOP(0x541DD7, 5, true);
            } else {
                injector::WriteMemory<unsigned char>(0x6194A0, 0xE9, true);
                injector::WriteMemoryRaw(0x541DD7, (char*)"\xE8\xE4\xD5\xFF\xFF", 5, true);
            }

            // ClearMouseStates
            injector::WriteMemory<float>(0xB73418 + 12, 0, true); // X
            injector::WriteMemory<float>(0xB73418 + 16, 0, true); // Y

            reinterpret_cast<void(__cdecl*)()>(0x541BD0)(); // CPad::ClearMouseHistory();
            reinterpret_cast<void(__cdecl*)()>(0x541DD0)(); // CPad::UpdatePads();
        } else if (gGameVer == eGameVer::VC) {
            if (ImGui::GetIO().MouseDrawCursor) {
                injector::WriteMemory<unsigned char>(0x6020A0, 0xC3, true);
                injector::MakeNOP(0x4AB6CA, 5, true);
            } else {
                injector::WriteMemory<unsigned char>(0x6020A0, 0x53, true);
                injector::WriteMemoryRaw(0x4AB6CA, (char*)"\xE8\x51\x21\x00\x00", 5, true);
            }

            // ClearMouseStates
            injector::WriteMemory<float>(0x94D788 + 8, 0, true); // X
            injector::WriteMemory<float>(0x94D788 + 12, 0, true);// Y

            reinterpret_cast<void(__cdecl*)()>(0x4ADB30)(); // CPad::ClearMouseHistory();
            reinterpret_cast<void(__cdecl*)()>(0x4AB6C0)(); // CPad::UpdatePads();
        } else if (gGameVer == eGameVer::III) {
            if (ImGui::GetIO().MouseDrawCursor) {
                injector::WriteMemory<unsigned char>(0x580D20, 0xC3, true);
                injector::MakeNOP(0x49272F, 5, true);
            } else {
                injector::WriteMemory<unsigned char>(0x580D20, 0x53, true);
                injector::WriteMemoryRaw(0x49272F, (char*)"\xE8\x6C\xF5\xFF\xFF", 5, true);
            }

            // ClearMouseStates
            injector::WriteMemory<float>(0x8809F0 + 8, 0, true); // X
            injector::WriteMemory<float>(0x8809F0 + 12, 0, true);// Y

            int pad = reinterpret_cast<int(__thiscall*)(int)>(0x492F60)(NULL); // CPad::GetPads();
            reinterpret_cast<void(__thiscall*)(int)>(0x491B50)(pad); // CPad::ClearMouseHistory();
            reinterpret_cast<void(__cdecl*)()>(0x492720)(); // CPad::UpdatePads();
        }
#endif

        curState = mouseVisible;
    }
}

static bool IsKeyPressed(int i, LPVOID data) {
    return reinterpret_cast<char*>(data)[i] & 0x80;
}

HRESULT CALLBACK Hook::hkGetDeviceState(IDirectInputDevice8* pThis, DWORD cbData, LPVOID lpvData) {
    HRESULT result = oGetDeviceState(pThis, cbData, lpvData);

    /*
    * We're detecting it here since usual WndProc doesn't seem to work for bully
    * This probably should work for other games using dinput too..?
    */
    ImGuiIO& io = ImGui::GetIO();
    if (ImGui::GetIO().MouseDrawCursor) {
        int frameCount = ImGui::GetFrameCount();
        if (cbData == 16) { // mouse
            LPDIMOUSESTATE2 mouseState = reinterpret_cast<LPDIMOUSESTATE2>(lpvData);

            // Block camera rotation
            mouseState->lX = 0;
            mouseState->lY = 0;
            io.MouseWheel = mouseState->lZ / static_cast<float>(WHEEL_DELTA);
            mouseState->lZ = 0;

            static int mouseCount = -1;
            if (frameCount != mouseCount) {
                io.MouseDown[0] = (mouseState->rgbButtons[0] != 0);
                io.MouseDown[1] = (mouseState->rgbButtons[1] != 0);
                mouseCount = frameCount;
            }

            // Block left & right clicks
            mouseState->rgbButtons[0] = 0;
            mouseState->rgbButtons[1] = 0;
        } else if (cbData == 256) { // keyboard
            /*
            *   GetDeviceData doesn't work
            */
            static int keyCount = -1;
            if (frameCount != keyCount) {
                io.KeyAlt = IsKeyPressed(DIK_LALT, lpvData) || IsKeyPressed(DIK_RALT, lpvData);
                io.KeyCtrl = IsKeyPressed(DIK_LCONTROL, lpvData) || IsKeyPressed(DIK_RCONTROL, lpvData);
                io.KeyShift = IsKeyPressed(DIK_LSHIFT, lpvData) || IsKeyPressed(DIK_RSHIFT, lpvData);

                for (size_t i = 0; i < cbData; ++i) {
                    bool pressed = IsKeyPressed(i, lpvData);
                    UINT vk = MapVirtualKeyEx(i, MAPVK_VSC_TO_VK, GetKeyboardLayout(NULL));

                    // ignore key holds
                    if (io.KeysDown[vk] != pressed) {
                        if (pressed) {
                            WCHAR c;
                            BYTE keystate[256];
                            ZeroMemory(keystate, 256);
                            ToUnicode(vk, i, keystate, &c, 1, 0);

                            // Capital letters on shift hold
                            if (io.KeyShift && c >= 0x61 && c <= 0x7A) {
                                c -= 0x20;
                            }
                            io.AddInputCharacterUTF16(c);
                        }
                        io.KeysDown[vk] = pressed;
                    }
                }
                keyCount = frameCount;
            }
        }
    }

    if (io.WantTextInput) {
        ZeroMemory(lpvData, 256);
        result = oGetDeviceState(pThis, cbData, lpvData);
    }
    return result;
}

bool Hook::GetDinputDevice(void** pMouse, size_t size) {
    if (!pMouse) {
        return false;
    }

    IDirectInput8* pDirectInput = NULL;

    // Create dummy device
    if (DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID*)&pDirectInput, NULL) != DI_OK) {
        return false;
    }

    LPDIRECTINPUTDEVICE8  lpdiInput;

    /*
    * We're creating a sysMouse but it still seems to receive keyboard messages?
    */
    if (pDirectInput->CreateDevice(GUID_SysMouse, &lpdiInput, NULL) != DI_OK) {
        pDirectInput->Release();
        return false;
    }

    lpdiInput->SetDataFormat(&c_dfDIKeyboard);
    lpdiInput->SetCooperativeLevel(GetActiveWindow(), DISCL_NONEXCLUSIVE);
    memcpy(pMouse, *reinterpret_cast<void***>(lpdiInput), size);
    lpdiInput->Release();
    pDirectInput->Release();
    return true;
}

BOOL CALLBACK Hook::hkSetCursorPos(int x, int y) {
    if (ImGui::GetIO().MouseDrawCursor) {
        return true;
    }
    return oSetCursorPos(x, y);
}

BOOL CALLBACK Hook::hkShowCursor(bool flag) {
    if (ImGui::GetIO().MouseDrawCursor) {
        return oShowCursor(TRUE);
    }
    return oShowCursor(flag);
}

void Hook::InputWatcher()
{
	while (true)
	{
		ImGuiIO& io = ImGui::GetIO();
        if (io.MouseDrawCursor) {
			io.MouseDown[0] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
			io.MouseDown[1] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
			io.MouseDown[2] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
        }
		Sleep(1);
	}
}

bool Hook::Inject(void *pCallback) {
    static bool injected;
    if (injected) {
        return false;
    }

    ImGui::CreateContext();
    MH_Initialize();
    PVOID pSetCursorPos = GetProcAddress(GetModuleHandle("user32.dll"), "SetCursorPos");
    PVOID pShowCursor = GetProcAddress(GetModuleHandle("user32.dll"), "ShowCursor");
    MH_CreateHook(pSetCursorPos, hkSetCursorPos, reinterpret_cast<LPVOID*>(&oSetCursorPos));
    MH_CreateHook(pShowCursor, hkShowCursor, reinterpret_cast<LPVOID*>(&oShowCursor));
    MH_EnableHook(pSetCursorPos);
    MH_EnableHook(pShowCursor);

    /*
        Must check for d3d9 first!
        Seems to crash with nvidia geforce experience overlay
        if anything else is checked before d3d9
    */
    if (GetModuleHandle("_gtaRenderHook.asi")
            || gGameVer == eGameVer::III_DE || gGameVer == eGameVer::VC_DE || gGameVer == eGameVer::SA_DE) {
        goto dx11;
    }

    if (init(kiero::RenderType::D3D9) == kiero::Status::Success) {
        gRenderer = eRenderer::Dx9;
        injected = true;
        kiero::bind(16, reinterpret_cast<LPVOID*>(&oReset), hkReset);
        kiero::bind(42, reinterpret_cast<LPVOID*>(&oEndScene), hkEndScene);
        pCallbackFunc = pCallback;
    }

    if (init(kiero::RenderType::OpenGL) == kiero::Status::Success) {
        gRenderer = eRenderer::OpenGL;
        injected = true;

        HMODULE hMod = GetModuleHandle("OPENGL32.dll");
        if (!hMod) {
            return false;
        }
        FARPROC addr = GetProcAddress(hMod, "wglSwapBuffers");
        MH_CreateHook(addr, hkGlSwapBuffer, reinterpret_cast<LPVOID*>(&oGlSwapBuffer));
        MH_EnableHook(addr);
        pCallbackFunc = pCallback;
    }

dx11:
    if (init(kiero::RenderType::D3D11) == kiero::Status::Success) {
        gRenderer = eRenderer::Dx11;
        kiero::bind(8, reinterpret_cast<LPVOID*>(&oPresent), hkPresent);
        kiero::bind(13, reinterpret_cast<LPVOID*>(&oResizeBuffers), hkResizeBuffers);
        pCallbackFunc = pCallback;
        injected = true;
    }

    if (gGameVer == eGameVer::BullySE) {
        static void *diMouse[32];
        if (GetDinputDevice(diMouse, sizeof(diMouse))) {
            MH_CreateHook(diMouse[9], hkGetDeviceState, reinterpret_cast<LPVOID*>(&oGetDeviceState));
            MH_EnableHook(diMouse[9]);
        }
    }

    if (gGameVer == eGameVer::IV) {
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InputWatcher), nullptr, 0, nullptr);
    }

    return injected;
}

void Hook::Remove() {
    pCallbackFunc = nullptr;
    SetWindowLongPtr(hwnd, -4, (LRESULT)oWndProc); // GWL_WNDPROC = -4

    if (gRenderer == eRenderer::Dx9) {
        ImGui_ImplDX9_Shutdown();
    } else if (gRenderer == eRenderer::Dx11) {
        ImGui_ImplDX11_Shutdown();
    } else {
        ImGui_ImplOpenGL3_Shutdown();
    }

    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    kiero::shutdown();
}
