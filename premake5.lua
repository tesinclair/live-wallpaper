
workspace "live-wallpaper"
    configurations { "Debug", "Release" }

project "live-wallpaper"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buidcfg}"

    includedirs {
        "third_party/*"
    }

    files { "src/**"" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"



