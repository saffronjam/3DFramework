-- ----------------------------------------------------
-- ----------------------------------------------------
-- --------- Saffron Engine Project Generator ---------
-- ----------------------------------------------------
-- ----------------------------------------------------


-- --------------------------------------
-- Saffron Workspace
-- --------------------------------------

workspace "Saffron"
	architecture "x64"
	
	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	startproject "Editor"

	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- --------------------------------------
-- Editor
-- --------------------------------------

group "Dependencies"
	include "Engine/ThirdParty/bgfx/premake5"
	include "Engine/ThirdParty/DirectXTK/premake5"
	include "Engine/ThirdParty/ImGui/premake5"
	include "Engine/ThirdParty/spdlog/premake5"
group ""

group "Engine"
include "Engine/premake5"
group ""

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	-- pchheader "SaffronPCH.h"
	-- pchsource "Engine/Source/SaffronPCH.cpp"
	
	files 
	{ 
		"%{prj.name}/Source/**.h", 
		"%{prj.name}/Source/**.c", 
		"%{prj.name}/Source/**.hpp", 
		"%{prj.name}/Source/**.cpp",
		"Directory.Build.props",	
	}
	
	includedirs
	{
		"%{prj.name}/Source",
		"Engine/Source",
		
		"Engine/ThirdParty/bgfx/include",
		"Engine/ThirdParty/DirectXTK/include",
		"Engine/ThirdParty/ImGui/include",
		"Engine/ThirdParty/Spdlog/include",
	}
	
	links
	{
		"Saffron",
	}
	
	filter "system:windows"
		systemversion "latest"
		
		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS",
			"SE_PLATFORM_WINDOWS"
		}
	
	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"	

group ""