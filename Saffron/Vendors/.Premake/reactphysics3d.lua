project "reactphysics3d"
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
        srcDirectory .. "include/reactphysics3d/**.h",
        srcDirectory .. "include/reactphysics3d/**.cpp",
        srcDirectory .. "src/**.h",
        srcDirectory .. "src/**.cpp"
    }

    includedirs
    {
        srcDirectory .. "include",
        srcDirectory .. "src"
    }
	
	disablewarnings 
	{ 
		"4018",
		"4099",
		"4244",
		"4267",
		"4305",
		"4715",
		"4996"
	}
    
    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        runtime "Release"
        optimize "on"