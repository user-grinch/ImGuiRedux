#pragma once
#include "imgui.h"
#include <vector>
#include <functional>
#include <time.h>

extern enum class eGameVer;
extern eGameVer gGameVer;

struct FontInfo {
    bool m_bFontLoaded = false;
    ImFont *m_pFont = nullptr;
    std::string m_Path;
    float m_nSize = 14.0f;
    size_t m_nStart, m_nEnd;
};

class ImGuiFrame {
public:
    ImGuiContext *m_pContext = nullptr;

    // Scaling related
    ImVec2 m_vecScaling = ImVec2(1, 1);
    bool m_bWasScalingUpdatedThisFrame;
    bool m_bNeedToUpdateScaling; 
    long long m_nLastScriptCallMS; 

    // Render buffers
    bool m_bIsBackBufferReady;
    std::vector<std::function<void()>> m_RenderBuffer, m_BackBuffer; 
    
    // for ImGui::ImageButton()
    ImVec4 m_vecImgTint = ImVec4(1, 1, 1, 1);
    ImVec4 m_vecImgBgCol = ImVec4(1, 1, 1, 1);

    // Fonts
    std::vector<std::pair<size_t, size_t>> m_FontGlyphRange;
    std::vector<FontInfo> m_FontTable;

    ImGuiFrame() {
        // m_pContext = ImGui::CreateContext();
    }

    ImGuiFrame& operator+=(std::function<void()> f) {
        if (!m_bIsBackBufferReady) {
            m_BackBuffer.push_back(f);
        }
        return *this;
    }   

    void BeforeRender() {
        // bool buildRequired = false;
        // for (auto& e: m_FontTable) {
        //     if (!e.m_bFontLoaded) {
        //         ImWchar ranges[] = { 
        //             e.m_nStart, e.m_nEnd, 0
        //         };
        //         ImGui::GetIO().Fonts->AddFontFromFileTTF(e.m_Path.c_str(), e.m_nSize, NULL, ranges);
        //         buildRequired = true;
        //     }
        // }

        // if (buildRequired) {
        //     ImGui::GetIO().Fonts->Build();
        // }
    }

    void OnRender() {
        for (auto func : m_RenderBuffer) {
            func();
        }

        // if back buffer is render ready switch the buffer and reset render state
        if (m_bIsBackBufferReady) {
            m_RenderBuffer = std::move(m_BackBuffer);
            m_bIsBackBufferReady = false;
        }

        time_t curTime = time(NULL);
        // Clear buffer when script stops responding
        bool scriptsPaused = false;
        switch(static_cast<int>(gGameVer)) {
            case 0: // III
                scriptsPaused = *(bool*)0x95CD7C;
                break;
            case 1: // VC
                scriptsPaused = *(bool*)0xA10B36;
                break;
            case 2: // SA
                scriptsPaused = *(bool*)0xB7CB49;
                break;
            default:
                break;
        }
        
        if (curTime-m_nLastScriptCallMS > 2 || scriptsPaused) {
            OnClear();
        }

        if (m_bWasScalingUpdatedThisFrame) {
            m_bNeedToUpdateScaling = false;
            m_bWasScalingUpdatedThisFrame = false;
        }
    }

    void OnClear() {
        m_RenderBuffer.clear();
    }
};