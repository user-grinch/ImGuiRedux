#pragma once
#include <vector>
#include <string>
#include "table.hpp"
#include "hook.h"
#include <functional>
#include <any>
#include <time.h>
#include <tuple>
#include "opcodemgr.h"
#include "notifypopup.h"
#include "imgui_internal.h"

extern enum class eGameVer;
extern eGameVer gGameVer;
extern ImGuiContext* GImGui;

class ScriptExData
{
private:
    ScriptExData(std::string id): ID(id){}

    /*
    * We're storing all the func ptrs that we need for a single frame
    * After drawing frame, we clear up the stored ptrs and repeat
    */
    struct ImGuiFrame
    {
    public:
        /*
        * Overlay colors for imgui images
        * Used by ButtonImage(..)
        */
        struct ImgOverlay
        {
            ImVec4 m_fTintCol = ImVec4(1, 1, 1, 1);
            ImVec4 m_fBgCol = ImVec4(1, 1, 1, 1);
        } m_ImGCol;

        bool m_bRender; // is backBuffer ready for render
        long long lastScriptCall; // last time script called ImGui::Begin(), hide if no script call

        std::vector<std::function<void()>> buf; // finished buffer for render
        std::vector<std::function<void()>> backBuf; // back buffer being processed
    
        ImGuiFrame& operator+=(std::function<void()> f)
        {
            // don't push more if back buffer is full
            if (!m_bRender)
            {
                backBuf.push_back(f);
            }
            return *this;
        }   

        void DrawFrames()
        {
            for (auto func : buf)
            {
                func();
            }

            // if back buffer is render ready switch the buffer and reset render state
            if (m_bRender)
            {
                buf = std::move(backBuf);
                m_bRender = false;
            }

            time_t curTime = time(NULL);

            // We're clearing our buffer if script isn't responding anymore
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
            }
            
            if (curTime-lastScriptCall > 2 || scriptsPaused) {
                ClearFrames();
            }
        }

        void ClearFrames()
        {
            buf.clear();
        }
    };
    /*
    *   Handles the return data between imgui frame & script
    */
    struct ComData
    {
        bool m_bPassed; // Was the data passed to the script, if yes, replace with new data 
        std::any m_Val;
    };

    std::string ID; // script indentifier
    /*
    * Cached return data of previous frame
    * Due to some limitations we can't run the ImGui realtime with the script
    * We run the ImGui frames independent of the script and cache the returns to return back to script
    */
    static inline Table<std::string, std::vector<ComData>> frameData;
    static inline std::vector<ScriptExData*> scripts; // ptr to all the scripts using ImGui
    static inline bool showCursor; // global cursor state flag
    static inline std::string curScriptID; // current script identifier
	static inline size_t m_nFramerate;
public:

    ImGuiFrame imgui;

    static size_t GetGameFPS()
    {
        return m_nFramerate;
    }

    static void SetCurrentScript(std::string id)
    {
        curScriptID = id;
    }

    static std::string GetCurrentScript()
    {
        return curScriptID;
    }
    
    static ScriptExData* Get()
    {
        // create the object if it doesn't exist
        for (auto it = scripts.begin(); it != scripts.end(); ++it)
        {
            // return the exisitng data
            if ((*it)->ID == curScriptID)
            {
                return *it;
            }
        }

        // return the new data
        ScriptExData* script = new ScriptExData(curScriptID);
        scripts.push_back(script);
        return script;
    }

    template<typename T>
    T GetData(const char* label, int index, T defaultVal)
    {
        try
        {
            ComData *pData = &frameData[label].at(index);
            T val = std::any_cast<T>(pData->m_Val);
            pData->m_bPassed = true;
            return val;
        }
        catch(...)
        {
            return defaultVal;
        }
    }
    template<typename T>
    void SetData(const char* label,size_t index, T val)
    {
        /*
        * Probably a shitty way to do this and gonna fk me later
        * But it works so..
        * 
        * We're only gonna set data if previous one was extracted!
        * This fixes an issue with race condition
        */
        if (frameData[label].size() < index+1)
        {
            frameData[label].push_back({false, val});
        }
        else
        {
            if (frameData[label].at(index).m_bPassed)
            {
                frameData[label].at(index) = {false, val};
            }
        }
    }

    static void SetCursorVisible(bool flag)
    {
        showCursor = flag;
    }

    static void DrawFrames()
    {
        // reset stuff
        showCursor = false;

        // draw frames
        for (auto it = scripts.begin(); it != scripts.end(); ++it)
        {
            (*it)->imgui.DrawFrames();
        }

        NotifiyPopup::Draw();
        // update stuff
        Hook::SetMouseState(showCursor);
        m_nFramerate = (size_t)ImGui::GetIO().Framerate;
    }

    // Clears all the internal stuff
    static void Clear() {
        scripts.clear();
        frameData.Clear();

        // Clear ImGui internal states here
        auto e = ImGui::GetCurrentContext();
        if (Hook::m_bInitialized && e) {
            e->Windows.clear_delete();
            e->WindowsFocusOrder.clear();
            e->WindowsTempSortBuffer.clear();
            e->CurrentWindow = NULL;
            e->CurrentWindowStack.clear();
            e->WindowsById.Clear();
            e->NavWindow = NULL;
            e->HoveredWindow = e->HoveredWindowUnderMovingWindow = NULL;
            e->ActiveIdWindow = e->ActiveIdPreviousFrameWindow = NULL;
            e->MovingWindow = NULL;
            e->ColorStack.clear();
            e->StyleVarStack.clear();
            e->OpenPopupStack.clear();
            e->BeginPopupStack.clear();

            e->TabBars.Clear();
            e->CurrentTabBarStack.clear();
            e->ShrinkWidthBuffer.clear();

            e->ClipperTempData.clear_destruct();

            e->Tables.Clear();
            e->TablesTempData.clear_destruct();
            e->DrawChannelsTempMergeBuffer.clear();

            e->ClipboardHandlerData.clear();
            e->MenusIdSubmittedThisFrame.clear();
            e->InputTextState.ClearFreeMemory();

            e->SettingsWindows.clear();
            e->SettingsHandlers.clear();
        }
    }
};