
workspace "live-wallpaper"
    configurations { "Debug", "Release" }

project "live-wallpaper"
    kind "ConsoleApp"
    language "C++"
    targetdir "bin/%{cfg.buidcfg}"

    files { "src/main.cpp", "src/lib/reader.cpp", "src/lib/reader.h", "src/lib/customErr.h" }

    filter "configurations:Debug"
        defines { "DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "NDEBUG" }
        optimize "On"



