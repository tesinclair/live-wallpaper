workspace "live-wallpaper"
    configurations { "Debug", "Release" }

    newoption {
        trigger = "verbose",
        description = "Build with all logging in release mode"
    }

    newoption {
        trigger = "silent",
        description = "Build will no logging in debug mode"
    }

project "live-wallpaper"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buildcfg}"

    includedirs {
        "third_party/*"
    }

    files { "src/**" }

    filter "configurations:Debug"
        defines { 
            "DEBUG",
            'ROOT="' .. _MAIN_SCRIPT_DIR .. '"'
        }
        symbols "On"

    filter { "configurations:Debug", "options:silent" }
        defines { "SILENT" }

    filter "configurations:Release" 
        defines { "NDEBUG" }
        optimize "On"

    filter { "configurations:Release", "options:verbose"}
        defines { "VERBOSE" }



