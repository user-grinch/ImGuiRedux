#pragma once
#include <vector>
#include <string>
#include "table.hpp"
#include "hook.h"
#include <any>
#include <tuple>
#include "opcodemgr.h"
#include "imgui_internal.h"
#include "imguiframe.hpp"
#include "notifypopup.h"

/* 
*   Handles data exchange between ImGui & CLEO
*   Repalce with new data when previous is passed
*/
struct ImGui2ScriptCom {
    bool m_bPassed; 
    std::any m_Val;
};

class ScriptExData {
private:
    static inline Table<std::string, std::vector<ImGui2ScriptCom>> m_FrameCache;
    static inline std::vector<ScriptExData*> m_pScriptTable;
    static inline bool m_bShowCursorFlag;
    static inline std::string m_CurrentScriptID;
	static inline size_t m_nFramerate;

    std::string m_ScriptID;

    ScriptExData(std::string id): m_ScriptID(id) {}

public:
    ImGuiFrame m_ImGuiData;

    static size_t GetGameFPS() {
        return m_nFramerate;
    }

    static void SetCurrentScript(std::string id) {
        m_CurrentScriptID = id;
    }

    static std::string GetCurrentScript() {
        return m_CurrentScriptID;
    }
    
    static ScriptExData* Get() {
        for (auto it = m_pScriptTable.begin(); it != m_pScriptTable.end(); ++it) {
            if ((*it)->m_ScriptID == m_CurrentScriptID) {
                return *it;
            }
        }

        ScriptExData* script = new ScriptExData(m_CurrentScriptID);
        m_pScriptTable.push_back(script);
        return script;
    }

    template<typename T>
    T GetData(const char* label, int index, T defaultVal) {
        try {
            ImGui2ScriptCom *pData = &m_FrameCache[label].at(index);
            T val = std::any_cast<T>(pData->m_Val);
            pData->m_bPassed = true;
            return val;
        } catch(...) {
            return defaultVal;
        }
    }

    template<typename T>
    void SetData(const char* label,size_t index, T val) {
        // Set data when previous one is retrived
        if (m_FrameCache[label].size() < index+1) {
            m_FrameCache[label].push_back({false, val});
        } else {
            if (m_FrameCache[label].at(index).m_bPassed) {
                m_FrameCache[label].at(index) = {false, val};
            }
        }
    }

    static void SetCursorVisible(bool flag) {
        m_bShowCursorFlag = flag;
    }

    static void InitRenderStates() {
        for (auto it = m_pScriptTable.begin(); it != m_pScriptTable.end(); ++it) {
            (*it)->m_ImGuiData.BeforeRender();
        }
    }

    static void RenderFrames() {
        m_bShowCursorFlag = false;

        for (auto it = m_pScriptTable.begin(); it != m_pScriptTable.end(); ++it) {
            (*it)->m_ImGuiData.OnRender();
        }

        NotifiyPopup::Draw();
        
        Hook::SetMouseState(m_bShowCursorFlag);
        m_nFramerate = (size_t)ImGui::GetIO().Framerate;
    }

    static void SetScaling(ImVec2 scaling) {
        for (auto it = m_pScriptTable.begin(); it != m_pScriptTable.end(); ++it) {
            (*it)->m_ImGuiData.m_vecScaling = scaling;
            (*it)->m_ImGuiData.m_bNeedToUpdateScaling = true;
        }
    }

    static void Clear() {
        m_pScriptTable.clear();
        m_FrameCache.Clear();
    }
};