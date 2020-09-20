project "Box2D"
    kind "StaticLib"
    language "C++"
    staticruntime "on"

	location "../%{prj.name}"
	
	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	srcDirectory = "../%{prj.name}/"
	
	targetdir ("../%{prj.name}/Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("../%{prj.name}/Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
    files
    {
		srcDirectory .. "include/box2d/**.h",
		srcDirectory .. "src/**.h",
		srcDirectory .. "src/**.cpp"
    }

    includedirs
    {
        srcDirectory .. "include",
        srcDirectory .. "src"
    }
    
    filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"