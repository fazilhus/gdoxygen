workspace "GDScript Parser"
    configurations { "Debug", "Release" }

    architecture "x86_64"

    location "./"

    startproject "Core"

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group ""
    include "Core"