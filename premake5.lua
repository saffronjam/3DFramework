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

	startproject "Saffron"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["assimp"] = "Saffron/Dependencies/assimp/include"
IncludeDir["Box2D"] = "Saffron/Dependencies/Box2D/include"
IncludeDir["entt"] = "Saffron/Dependencies/entt/include"
IncludeDir["FastNoise"] = "Saffron/Dependencies/FastNoise/include"
IncludeDir["Glad"] = "Saffron/Dependencies/Glad/include"
IncludeDir["GLFW"] = "Saffron/Dependencies/GLFW/include"
IncludeDir["glm"] = "Saffron/Dependencies/glm/include"
IncludeDir["ImGui"] = "Saffron/Dependencies/ImGui"
IncludeDir["mono"] = "Saffron/Dependencies/mono/include"
IncludeDir["reactphysics3d"] = "Saffron/Dependencies/reactphysics3d/include"
IncludeDir["spdlog"] = "Saffron/Dependencies/spdlog/include"
IncludeDir["stb"] = "Saffron/Dependencies/stb/include"
IncludeDir["yamlcpp"] = "Saffron/Dependencies/yaml-cpp/include"

LibraryDir = {}
LibraryDir["mono"] = "Dependencies/mono/lib/Debug/mono-2.0-sgen.lib"
LibraryDir["assimp_deb"] = "Dependencies/assimp/lib/Debug/assimp-vc141-mtd.lib"
LibraryDir["assimp_rel"] = "Dependencies/assimp/lib/Release/assimp-vc141-mt.lib"

SharedLibraryDir = {}
SharedLibraryDir["mono"] = "Dependencies/mono/bin/Debug/mono-2.0-sgen.dll"
SharedLibraryDir["assimp_deb"] = "Dependencies/assimp/bin/Debug/assimp-vc141-mtd.dll"
SharedLibraryDir["assimp_rel"] = "Dependencies/assimp/bin/Release/assimp-vc141-mt.dll"

RuntimeLibararyDir = {}
RuntimeLibararyDir["mono"] = "Dependencies/mono/bin/Runtime"

outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"



-- --------------------------------------
-- Engine
-- --------------------------------------

group "Engine"

group "Engine/Dependencies"
	include "Saffron/Dependencies/assimp/premake5"
	include "Saffron/Dependencies/Box2D/premake5"
	include "Saffron/Dependencies/entt/premake5"
	include "Saffron/Dependencies/FastNoise/premake5"
	include "Saffron/Dependencies/Glad/premake5"
	include "Saffron/Dependencies/GLFW/premake5"
	include "Saffron/Dependencies/glm/premake5"
	include "Saffron/Dependencies/ImGui/premake5"
	include "Saffron/Dependencies/mono/premake5"
	include "Saffron/Dependencies/reactphysics3d/premake5"
	include "Saffron/Dependencies/spdlog/premake5"
	include "Saffron/Dependencies/stb/premake5"
	include "Saffron/Dependencies/yaml-cpp/premake5"

group "Engine"

-- --------------------------------------
-- Saffron
-- --------------------------------------

project "Saffron"
	location "Saffron"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	pchheader "SaffronPCH.h"
	pchsource "Saffron/Source/SaffronPCH.cpp"

	files 
	{ 
		"%{prj.name}/Source/**.h", 
		"%{prj.name}/Source/**.c", 
		"%{prj.name}/Source/**.hpp", 
		"%{prj.name}/Source/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Source",
		"%{IncludeDir.assimp}",
		"%{IncludeDir.Box2D}",
		"%{IncludeDir.entt}",
		"%{IncludeDir.FastNoise}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.mono}",
		"%{IncludeDir.reactphysics3d}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.stb}",
		"%{IncludeDir.yamlcpp}"
	}
	
	links 
	{ 
		"Box2D",
		"FastNoise",
		"Glad",
        "GLFW",
		"ImGui",
		"opengl32.lib",
		"reactphysics3d",
		"yaml-cpp",
		"%{LibraryDir.mono}"
	}
	
	postbuildcommands 
	{
		'{COPY} "Resources/Assets" "%{cfg.targetdir}/Resources/Assets"',
		'{COPY} "%{SharedLibraryDir.mono}" "%{cfg.targetdir}"',
		'{COPY} "%{RuntimeLibararyDir.mono}" "%{cfg.targetdir}/Runtime"'
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

		links
		{
			"%{LibraryDir.assimp_deb}"
		}

		postbuildcommands 
		{
			'{COPY} "%{SharedLibraryDir.assimp_deb}" "%{cfg.targetdir}"'
		}
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"

		links
		{
			"%{LibraryDir.assimp_rel}"
		}

		postbuildcommands 
		{
			'{COPY} "%{SharedLibraryDir.assimp_rel}" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"

		links
		{
			"%{LibraryDir.assimp_rel}"
		}

		postbuildcommands
		{		
			'{COPY} "%{SharedLibraryDir.assimp_rel}" "%{cfg.targetdir}"'
		}


-- --------------------------------------
-- ScriptCore
-- --------------------------------------

project "ScriptCore"
	location "ScriptCore"
	kind "SharedLib"
	language "C#"

	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/Source/**.cs", 
	}
group ""

-- --------------------------------------
-- Games
-- --------------------------------------


-- Later on this does not need to be in a separate editor (since the engine editor will not be run for Visual Studios debugger)
-- workspace "Games"
	-- architecture "x64"
	-- targetdir "build"
	
	-- configurations 
	-- { 
		-- "Debug", 
		-- "Release",
		-- "Dist"
	-- }
------------------------------------------------------------------------------------------------------------------------------

group "Games"

-- This should be an updating list so that every project active in the editor is visible
-- Like:
-- project "Game1"
-- ...
-- project "Game2"
-- ...

project "2DGameProject"
	location "Games/2DGameProject"
	kind "SharedLib"
	language "C#"
	
	targetdir ("Saffron/Resources/Assets/Scripts")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	files 
	{
		"Games/%{prj.name}/Source/**.cs", 
	}
	links
	{
		"ScriptCore"
	}
	
project "EmptyWorldProject"
	location "Games/EmptyWorldProject"
	kind "SharedLib"
	language "C#"
	
	targetdir ("Saffron/Resources/Assets/Scripts")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	files 
	{
		"Games/%{prj.name}/Source/**.cs", 
	}
	links
	{
		"ScriptCore"
	}
group ""