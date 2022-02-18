#pragma once
#include <vector>
#include <string>
#include "depend/cleo/cleo_redux_sdk.h"
#include "d3dhook.h"
#include <functional>
#include <any>
#include "opcodemgr.h"

class ScriptMgr;

class ScriptData
{
private:
    typedef std::function<void()> VoidFunc;
    typedef std::vector<std::any> AnyVector;
    /*
    * ImGui Func return data is stored here
    */
    struct Cache
    {
        std::string id = "";
        AnyVector data;
    };
    Context* ctx;
    std::vector<VoidFunc> vecFunc;  
    std::vector<Cache> vecCache;

    friend ScriptMgr;
public:

    ScriptData(Context* cx) : ctx(cx){}

    ScriptData& operator+=(VoidFunc f)
    {
        vecFunc.push_back(f);
        return *this;
    }
    /*
    * Function to fetch values saved inside the cache
    * We use this to return cached values from previous frame
    */
    template<typename T>
    T Get(const char* label, int index, T defaultVal)
    {
        try
        {
            for (const auto& it : vecCache)
            {
                // value exists, return ref
                if (it.id == std::string(label))
                {
                    return std::any_cast<T>(it.data.at(index));
                }
            }

            return defaultVal;
        }
        catch (...)
        {
            return defaultVal;
        }
    }
    /*
    * Function to save values inside the cache
    */
    template<typename T>
    void Set(const char* label, size_t index, T val)
    {
        /*
        * Probably a shitty way to do this and gonna fk me later
        * But it works so..
        */
        std::string id(label);
        for (auto cache : vecCache)
        {
            if (cache.id == id)
            {
                if (cache.data.size() < index + 1)
                {
                    cache.data.push_back(val);
                }
                else
                {
                    cache.data[index] = val;
                }
                return;
            }
        }

        // script doesn't exist in vector, so add it
        vecCache.push_back({ id, {val} });
    }
};

/*
* Script Manaager Class
* Handle pointers to script context and their data
*/
class ScriptMgr
{
private:
    static inline std::vector<ScriptData> vecScripts;
    ScriptMgr() {}
    
public:
    static ScriptData* Get(Context* cx)
    {
        static ScriptMgr mgr;

        for (auto& scpt : vecScripts)
        {
            if (scpt.ctx == cx)
            {
                return &scpt;
            }
        }

        vecScripts.push_back(cx);
        return &vecScripts.back();
    }

    void SetCursorState(bool visible)
    {
        //show_cursor = flag;
    }

    static void ProcessFrame()
    {
        // reset stuff
        OpcodeMgr::nFPS = (size_t)ImGui::GetIO().Framerate;
        OpcodeMgr::frameHeight = ImGui::GetFrameHeight();

        // Looping through the script pool
        for (auto& script : vecScripts)
        {
            // calling the functions to draw the frame
            for (auto& func : script.vecFunc)
            {
                func();
            }

            // clear the frame vector
            script.vecFunc.clear();
        }

        // update stuff
        D3dHook::SetMouseState(true);
    }
};