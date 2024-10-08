workspace "GDScript Parser"
    configurations { "Debug", "Release" }

    architecture "x86_64"

    location "./"

    startproject "App"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group ""
    include "Core"
    include "App"

group "Tests"
    include "Tests"
