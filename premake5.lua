workspace "live-wallpaper"
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
