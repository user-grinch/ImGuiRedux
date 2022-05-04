#pragma once
#include <vector>
#include <string>
#include "cleo_redux_sdk.h"
#include "table.hpp"
#include "hook.h"
#include <functional>
#include <any>
#include <tuple>
#include "opcodemgr.h"

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
        bool m_bRender; // is backBuffer ready for render

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
        }

        void ClearFrames()
        {
            buf.clear();
        }
    };
	static inline size_t m_nFramerate;
    std::string ID; // script indentifier
    /*
    * Cached return data of previous frame
    * Due to some limitations we can't run the ImGui realtime with the script
    * We run the ImGui frames independent of the script and cache the returns to retun back to script
    */
    Table<std::string, std::vector<std::any>> frame_cache;
    static inline std::vector<ScriptExData*> scripts; // ptr to all the scripts using ImGui
    static inline bool showCursor; // global cursor state flag
    static inline std::string curScriptID; // current script identifier

public:

    ImGuiFrame imgui;

    static size_t GetGameFPS()
    {
        return m_nFramerate;
    }

    static void SetCurrentScript(std::string id)
    {
        if (id == "")
        {
            curScriptID = id;
        }
        else
        {
            curScriptID = id;
        }
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
            return  std::any_cast<T>(frame_cache[label].at(index));
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
        */
        if (frame_cache[label].size() < index+1)
        {
            frame_cache[label].push_back(val);
        }
        else
        {
            frame_cache[label].at(index) = val;
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

        // update stuff
        Hook::SetMouseState(showCursor);
        m_nFramerate = (size_t)ImGui::GetIO().Framerate;
    }
};