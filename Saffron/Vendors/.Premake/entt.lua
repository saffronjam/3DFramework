project "Glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"

	location "../%{prj.name}"
	
	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	srcDirectory = "../%{prj.name}/"
	
	targetdir ("../%{prj.name}/Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("../%{prj.name}/Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
    files
    {
        srcDirectory .. "include/%{prj.name}/entt.hpp"
    }

    includedirs
    {
        srcDirectory .. "include"
    }
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"