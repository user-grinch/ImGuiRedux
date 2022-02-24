#pragma once
#include <vector>
#include <string>
#include "cleo/cleo_redux_sdk.h"
#include "table.hpp"
#include "d3dhook.h"
#include <functional>
#include <any>
#include <tuple>
#include "opcodemgr.h"

class ScriptExData
{
private:
    ScriptExData(Context* data): context(data){}

    /*
    * We're storing all the func ptrs that we need for a single frame
    * After drawing frame, we clear up the stored ptrs and repeat
    */
    struct ImGuiFrame
    {
        std::vector<std::function<void()>> frames;

        ImGuiFrame& operator+=(std::function<void()> f)
        {
            frames.push_back(f);
            return *this;
        }

        void DrawFrames()
        {
            for (auto func : frames)
            {
                func();
            }
        }

        void ClearFrames()
        {
            frames.clear();
        }
    };

    Context* context; // script indentifier
    /*
    * Cached return data of previous frame
    * Due to some limitations we can't run the ImGui realtime with the script
    * We run the ImGui frames independent of the script and cache the returns to retun back to script
    */
    Table<std::string, std::vector<std::any>> frame_cache;
    static inline std::vector<ScriptExData*> scripts; // ptr to all the scripts using ImGui
    static inline bool show_cursor; // global cursor state flag

public:

    ImGuiFrame imgui;
    static ScriptExData* Get(Context* cx)
    {
        // create the object if it doesn't exist
        for (auto it = scripts.begin(); it != scripts.end(); ++it)
        {
            // return the exisitng data
            if ((*it)->context == cx)
            {
                return *it;
            }
        }

        // return the new data
        ScriptExData* script = new ScriptExData(cx);
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
        show_cursor = flag;
    }

    static void DrawFrames()
    {
        // reset stuff
        show_cursor = false;
        ImGuiStyle::ImGuiStyle();

        OpcodeMgr::nFPS = (size_t)ImGui::GetIO().Framerate;
        OpcodeMgr::frameHeight = ImGui::GetFrameHeight();

        // draw frames
        for (auto it = scripts.begin(); it != scripts.end(); ++it)
        {
            (*it)->imgui.DrawFrames();
            (*it)->imgui.ClearFrames();
        }

        // update stuff
        D3dHook::SetMouseState(show_cursor);
    }
};