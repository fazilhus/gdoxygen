project "Core"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "off"

    files {
        "**.hpp",
        "**.cpp",
    }

    targetdir ("%{wks.location}/build/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/build/obj/" .. outputdir .. "/%{prj.name}")

    filter { "system:windows" }
        defines { "WIN" }
    filter {}

    filter { "configurations:Debug" }
        defines { "DEBUG" }
        symbols "On"
    filter {}

    filter { "configurations:Release" }
        defines { "RELEASE" }
        optimize "On"
    filter {}