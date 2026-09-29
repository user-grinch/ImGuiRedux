#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <any>
#include <type_traits>
#include "hook.h"
#include "opcodemgr.h"
#include "imgui_internal.h"
#include "imguiframe.hpp"
#include "notifypopup.h"

/* 
*   Handles thread-safe data exchange between ImGui (render thread) & CLEO (game thread)
*/
class ScriptExData {
private:
    static inline std::mutex m_CacheMutex;
    static inline std::recursive_mutex m_ScriptTableMutex;
    static inline std::unordered_map<std::string, std::vector<std::any>> m_DataCache;
    static inline std::unordered_map<std::string, std::vector<bool>> m_EventCache;
    static inline std::vector<ScriptExData*> m_pScriptTable;
    static inline std::atomic<bool> m_bShowCursorFlag = false;
    static inline std::string m_CurrentScriptID;
    static inline size_t m_nFramerate = 0;

    std::string m_ScriptID;

    ScriptExData(std::string id): m_ScriptID(id) {}

public:
    ImGuiFrame m_ImGuiData;

    static size_t GetGameFPS() {
        return m_nFramerate;
    }

    static void SetCurrentScript(std::string id) {
        std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
        m_CurrentScriptID = id;
    }

    static std::string GetCurrentScript() {
        std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
        return m_CurrentScriptID;
    }
    
    static ScriptExData* Get() {
        std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
        for (auto* script : m_pScriptTable) {
            if (script->m_ScriptID == m_CurrentScriptID) {
                return script;
            }
        }

        ScriptExData* script = new ScriptExData(m_CurrentScriptID);
        m_pScriptTable.push_back(script);
        return script;
    }

    // Set momentary pulse event (e.g. button click)
    // Latches true when clicked until consumed by the script
    void SetEvent(const std::string& label, size_t index, bool val) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto& vec = m_EventCache[label];
        if (vec.size() <= index) {
            vec.resize(index + 1, false);
        }
        if (val) {
            vec[index] = true;
        }
    }

    // Get and consume momentary pulse event
    bool GetEvent(const std::string& label, size_t index, bool defaultVal = false) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto it = m_EventCache.find(label);
        if (it != m_EventCache.end() && index < it->second.size()) {
            bool val = it->second[index];
            it->second[index] = false; // consume event
            return val;
        }
        return defaultVal;
    }

    template<typename T>
    T GetData(const std::string& label, size_t index, T defaultVal) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        if constexpr (std::is_same_v<T, bool>) {
            auto evIt = m_EventCache.find(label);
            if (evIt != m_EventCache.end() && index < evIt->second.size() && evIt->second[index]) {
                evIt->second[index] = false;
                return true;
            }
        }
        auto it = m_DataCache.find(label);
        if (it != m_DataCache.end() && index < it->second.size()) {
            try {
                return std::any_cast<T>(it->second[index]);
            } catch (...) {
                return defaultVal;
            }
        }
        return defaultVal;
    }

    template<typename T>
    void SetData(const std::string& label, size_t index, T val) {
        std::lock_guard<std::mutex> lock(m_CacheMutex);
        auto& vec = m_DataCache[label];
        if (vec.size() <= index) {
            vec.resize(index + 1);
        }
        vec[index] = val;
    }

    static void SetCursorVisible(bool flag) {
        m_bShowCursorFlag = flag;
    }

    static void InitRenderStates() {
        std::vector<ScriptExData*> scripts;
        {
            std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
            scripts = m_pScriptTable;
        }
        for (auto* script : scripts) {
            script->m_ImGuiData.BeforeRender();
        }
    }

    static void RenderFrames() {
        m_bShowCursorFlag = false;

        std::vector<ScriptExData*> scripts;
        {
            std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
            scripts = m_pScriptTable;
        }

        for (auto* script : scripts) {
            script->m_ImGuiData.OnRender();
        }

        NotifiyPopup::Draw();
        
        Hook::SetMouseState(m_bShowCursorFlag.load());
        m_nFramerate = (size_t)ImGui::GetIO().Framerate;
    }

    static void SetScaling(ImVec2 scaling) {
        std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
        for (auto* script : m_pScriptTable) {
            script->m_ImGuiData.m_vecScaling = scaling;
            script->m_ImGuiData.m_bNeedToUpdateScaling = true;
        }
    }

    static void Clear() {
        {
            std::lock_guard<std::recursive_mutex> lock(m_ScriptTableMutex);
            for (auto* script : m_pScriptTable) {
                delete script;
            }
            m_pScriptTable.clear();
        }
        {
            std::lock_guard<std::mutex> lock(m_CacheMutex);
            m_DataCache.clear();
            m_EventCache.clear();
        }
    }
};