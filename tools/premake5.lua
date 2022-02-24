
----------------------------
-- Premake Project Generator
----------------------------

-- Environment vars
----------------------------
-- Should get picked up automatically if you installed them properly
DX9SDK_DIR = os.getenv("DIRECTX9_SDK_DIR")

if (DX9SDK_DIR == nil) then
    error("DIRECTX9_SDK_DIR environment variable not set")
end

----------------------------

workspace "ImGuiRedux"
    configurations { "Debug", "Release" }
    architecture "x86"
    platforms "Win32"
    language "C++"
    cppdialect "C++20"
    characterset "MBCS"
    staticruntime "On"
    location "../build"
    targetdir "../build/bin"

project "depend"
    kind "StaticLib"

    files { 
        "../depend/**.h", 
        "../depend/**.hpp", 
        "../depend/**.c", 
        "../depend/**.cpp" 
    }

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

project "ImGuiRedux"
    kind "SharedLib"
    targetextension ".cleo"
    
    files { 
        "../src/**.h", 
        "../src/**.hpp", 
        "../src/**.cpp" 
    }
    includedirs {
        "../depend/"
    }

    libdirs {
        "../depend/libs/",
    }
    
    defines { 
        "NDEBUG", 
        "_DX9_SDK_INSTALLED",
    }

    pchheader "pch.h"
    pchsource "../src/pch.cpp"

    filter "configurations:Debug"
        symbols "On"
        links { 
            "depend",
            "d3d9",
            "d3d11",
            "cleo_redux"
        }

    filter "configurations:Release"
        optimize "On"
        links { 
            "depend",
            "d3d9",
            "d3d11",
            "cleo_redux"
        }
        