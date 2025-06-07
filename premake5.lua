workspace "live-wallpaper"
    configurations { "Debug", "Release", "Test" }
    
project "live-wallpaper"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"
    
    files { 
        "src/main.cpp",
        -- libs --
        "src/lib/reader.cpp",
        "src/lib/reader.h",
        "src/lib/customErr.h"
    }
    
    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"
        
    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"
        
    filter "configurations:Test"
        targetname "test-live-wallpaper"
        removefiles { "src/main.cpp" }
        files {
            -- test files --
            "tests/**.test.cpp",
            "tests/**.test.h"
        }
        defines { "DEBUG" }
        symbols "On"
        
    filter {} -- Reset filter
