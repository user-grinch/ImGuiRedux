workspace "ImGuiRedux"
    configurations { 
        "Debug", 
        "Release" 
    }

    platforms {
        "Win32",
        "Win64"
    }
    language "C++"
    cppdialect "C++latest"
    characterset "MBCS"
    staticruntime "On"
    location "../build"
    targetdir "../build/bin"
    kind "SharedLib"
    targetextension ".cleo"

    files { 
        "../include/**", 
        "../src/**"
    }
    
    includedirs {
        "../include/**"
    }

    libdirs {
        "../lib/"
    }

project "ImGuiRedux"
    filter { "platforms:Win32" }
        targetname "ImGuiReduxWin32"
        architecture "x86"
        links { 
            "d3d9",
            "d3dx9",
            "d3d11",
            "cleo_redux",
            "libMinHook-x86",
            "dinput8.lib",
            "dxguid.lib"
        }
    
    filter { "platforms:Win64" }
        targetname "ImGuiReduxWin64"
        architecture "x64"
        links { 
            "d3d9",
            "d3dx9x64",
            "d3d11",
            "cleo_redux64",
            "libMinHook-x64",
            "dinput8.lib",
            "dxguid.lib"
        }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"
        defines {
            "NDEBUG"
        }