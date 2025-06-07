workspace "live-wallpaper"
<<<<<<< HEAD
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
=======
    configurations { "Debug", "Release" }

    newoption {
        trigger = "test",
        description = "Build tests"
    }

if not _OPTIONS["test"] then
    project "live-wallpaper"
        kind "ConsoleApp"
        language "C++"
        targetdir "bin/%{cfg.buidcfg}"

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
end

if _OPTIONS["test"] then
    project "test-live-wallpaper"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}/tests"

    files { 
        -- test files --
        "tests/**.test.cpp",
        "tests/**.test.h",
        -- lib files for testing -- 
        "src/lib/reader.cpp",
        "src/lib/reader.h",
        "src/lib/customErr.h" 
    }

    defines { "DEBUG" }
    symbols "On"
end
>>>>>>> 83bd562 (adds main test file and adds testing options to premake)
