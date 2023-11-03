workspace "ImGuiRedux"
    configurations { 
        "Debug", 
        "Release", 
        "DebugCleo", 
        "ReleaseCleo"
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
        architecture "x86"
        links { 
            "d3d9",
            "d3dx9",
            "d3d11",
            "delayimp",
            "libMinHook-x86",
            "dinput8.lib",
            "dxguid.lib"
        }
        filter "configurations:DebugCleo"
            targetname "ImGuiCleoWin32"
            symbols "On"
            defines {
                "RUNTIME_CLEO" 
            }
            links { 
                "CLEO",
            }

        filter "configurations:ReleaseCleo"
            targetname "ImGuiCleoWin32"
            optimize "On"
            defines {
                "NDEBUG",
                "RUNTIME_CLEO" 
            }
            links { 
                "CLEO",
            }
        
        filter "configurations:Debug"
            targetname "ImGuiReduxWin32"
            symbols "On"
            defines {
                "RUNTIME_REDUX" 
            }
            links { 
                "cleo_redux",
            }

        filter "configurations:Release"
            targetname "ImGuiReduxWin32"
            optimize "On"
            defines {
                "NDEBUG",
                "RUNTIME_REDUX" 
            }
            links { 
                "cleo_redux",
            }
    
    filter { "platforms:Win64" }
        targetname "ImGuiReduxWin64"
        architecture "x64"
        links { 
            "d3d9",
            "d3dx9x64",
            "d3d11",
            "delayimp",
            "cleo_redux64",
            "libMinHook-x64",
            "dinput8.lib",
            "dxguid.lib"
        }

        filter "configurations:Debug"
            symbols "On"
            defines {
                "RUNTIME_REDUX" 
            }

        filter "configurations:Release"
            optimize "On"
            defines {
                "NDEBUG",
                "RUNTIME_REDUX" 
            }
