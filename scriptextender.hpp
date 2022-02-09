#pragma once
#include <vector>
#include <string>
#include "depend/cleo/cleo_redux_sdk.h"
#include "table.hpp"
#include "d3dhook.h"
#include <functional>

class ScriptExData
{
private:
    ScriptExData(Context* data): data(data){}

public:
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
    } imgui;

    Context* data;
    Table<std::string,float> frame_cache; // Cached data of previous frame
    static inline std::vector<ScriptExData*> scripts;
    static inline bool show_cursor;

    static ScriptExData* Get(Context* cx)
    {
        // create the object if it doesn't exist
        for (auto it = scripts.begin(); it != scripts.end(); ++it)
        {
            // return the exisitng data
            if ((*it)->data == cx)
            {
                return *it;
            }
        }

        // return the new data
        ScriptExData* script = new ScriptExData(cx);
        scripts.push_back(script);
        return script;
    }

    static void DrawFrames()
    {
        // reset stuff
        show_cursor = true;
        ImGuiStyle::ImGuiStyle();

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