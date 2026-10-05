#pragma once
#include "imgui.h"
#include <vector>
#include <functional>
#include <mutex>
#include <windows.h>
#include <string>

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
private:
    std::mutex m_BufferMutex;
    std::vector<std::function<void()>> m_BuildingBuffer;
    std::vector<std::function<void()>> m_BackBuffer;
    std::vector<std::function<void()>> m_RenderBuffer;
    bool m_bHasNewFrame = false;

public:
    ImGuiContext *m_pContext = nullptr;

    // Scaling related
    ImVec2 m_vecScaling = ImVec2(1, 1);
    bool m_bWasScalingUpdatedThisFrame = false;
    bool m_bNeedToUpdateScaling = false; 
    uint64_t m_nLastScriptCallMS = 0; 

    // Render buffer readiness flag (for backwards compatibility)
    bool m_bIsBackBufferReady = false;
    
    // for ImGui::ImageButton()
    ImVec4 m_vecImgTint = ImVec4(1, 1, 1, 1);
    ImVec4 m_vecImgBgCol = ImVec4(1, 1, 1, 1);

    // Fonts
    std::vector<std::pair<size_t, size_t>> m_FontGlyphRange;
    std::vector<FontInfo> m_FontTable;

    ImGuiFrame() {
        m_nLastScriptCallMS = GetTickCount64();
    }

    void BeginFrame() {
        m_BuildingBuffer.clear();
        m_nLastScriptCallMS = GetTickCount64();
    }

    ImGuiFrame& operator+=(std::function<void()> f) {
        if (m_BuildingBuffer.size() < 10000) {
            m_BuildingBuffer.push_back(std::move(f));
        }
        return *this;
    }   

    void EndFrame() {
        {
            std::lock_guard<std::mutex> lock(m_BufferMutex);
            m_BackBuffer = std::move(m_BuildingBuffer);
            m_bHasNewFrame = true;
            m_bIsBackBufferReady = true;
        }
        m_nLastScriptCallMS = GetTickCount64();
    }

    void BeforeRender() {
    }

    void OnRender() {
        {
            std::lock_guard<std::mutex> lock(m_BufferMutex);
            if (m_bHasNewFrame) {
                m_RenderBuffer = std::move(m_BackBuffer);
                m_bHasNewFrame = false;
                m_bIsBackBufferReady = false;
            }
        }

        uint64_t curTime = GetTickCount64();
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
        
        if ((curTime - m_nLastScriptCallMS > 2500) || scriptsPaused) {
            OnClear();
            return;
        }

        for (const auto& func : m_RenderBuffer) {
            if (func) {
                func();
            }
        }

        if (m_bWasScalingUpdatedThisFrame) {
            m_bNeedToUpdateScaling = false;
            m_bWasScalingUpdatedThisFrame = false;
        }
    }

    void OnClear() {
        std::lock_guard<std::mutex> lock(m_BufferMutex);
        m_RenderBuffer.clear();
        m_BackBuffer.clear();
        m_BuildingBuffer.clear();
        m_bHasNewFrame = false;
        m_bIsBackBufferReady = false;
    }
};