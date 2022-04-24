
----------------------------
-- Premake Project Generator
----------------------------

workspace "ImGuiRedux"
    configurations { "Debug", "Release" }
    platforms "Win32"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    staticruntime "On"
    location "../build"
    targetdir "../build/bin"

project "ImGuiRedux"
    architecture "x86"
    kind "SharedLib"
    targetextension ".cleo"
    
    files { 
        "../include/**.h", 
        "../include/**.hpp", 
        "../include/**.c", 
        "../include/**.cpp", 
        "../src/**.h", 
        "../src/**.hpp", 
        "../src/**.cpp" 
    }
    includedirs {
        "../include/"
    }

    libdirs {
        "../lib/",
    }
    
    defines { 
        "NDEBUG", 
        "_DX9_SDK_INSTALLED",
    }

    filter "configurations:Debug"
        symbols "On"
        links { 
            "d3d9",
            "d3d11",
            "cleo_redux",
            "libMinHook-x86"
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "d3d9",
            "d3d11",
            "cleo_redux",
            "libMinHook-x86"
        }

project "ImGuiRedux64"
    architecture "x64"
    kind "SharedLib"
    targetextension ".cleo"
    
    files { 
        "../include/**.h", 
        "../include/**.hpp", 
        "../include/**.c", 
        "../include/**.cpp",
        "../src/**.h", 
        "../src/**.hpp", 
        "../src/**.cpp" 
    }
    includedirs {
        "../include/"
    }

    libdirs {
        "../lib/",
    }
    
    defines { 
        "NDEBUG", 
        "_DX9_SDK_INSTALLED",
    }

    filter "configurations:Debug"
        symbols "On"
        links { 
            "d3d9",
            "d3d11",
            "cleo_redux64",
            "libMinHook-x64"
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "d3d9",
            "d3d11",
            "cleo_redux64",
            "libMinHook-x64"
        }
        