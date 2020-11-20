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
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["assimp"] = "Saffron/Vendors/assimp/include"
IncludeDir["Box2D"] = "Saffron/Vendors/Box2D/include"
IncludeDir["entt"] = "Saffron/Vendors/entt/include"
IncludeDir["FastNoise"] = "Saffron/Vendors/FastNoise/include"
IncludeDir["Glad"] = "Saffron/Vendors/Glad/include"
IncludeDir["GLFW"] = "Saffron/Vendors/GLFW/include"
IncludeDir["glm"] = "Saffron/Vendors/glm/include"
IncludeDir["ImGui"] = "Saffron/Vendors/ImGui"
IncludeDir["mono"] = "Saffron/Vendors/mono/include"
IncludeDir["reactphysics3d"] = "Saffron/Vendors/reactphysics3d/include"
IncludeDir["spdlog"] = "Saffron/Vendors/spdlog/include"
IncludeDir["stb"] = "Saffron/Vendors/stb/include"
IncludeDir["yamlcpp"] = "Saffron/Vendors/yaml-cpp/include"

LibraryDir = {}
LibraryDir["mono"] = "Vendors/mono/lib/Debug/mono-2.0-sgen.lib"
LibraryDir["assimp_deb"] = "Vendors/assimp/lib/Debug/assimp-vc141-mtd.lib"
LibraryDir["assimp_rel"] = "Vendors/assimp/lib/Release/assimp-vc141-mt.lib"

group "Dependencies"
	include "Saffron/Vendors/.Premake/assimp"
	include "Saffron/Vendors/.Premake/Box2D"
	include "Saffron/Vendors/.Premake/entt"
	include "Saffron/Vendors/.Premake/FastNoise"
	include "Saffron/Vendors/.Premake/Glad"
	include "Saffron/Vendors/.Premake/GLFW"
	include "Saffron/Vendors/.Premake/glm"
	include "Saffron/Vendors/.Premake/ImGui"
	include "Saffron/Vendors/.Premake/mono"
	include "Saffron/Vendors/.Premake/reactphysics3d"
	include "Saffron/Vendors/.Premake/spdlog"
	include "Saffron/Vendors/.Premake/stb"
	include "Saffron/Vendors/.Premake/yaml-cpp"
group ""


-- --------------------------------------
-- Engine
-- --------------------------------------

group "Engine"


-- --------------------------------------
-- Saffron
-- --------------------------------------

project "Saffron"
	location "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	pchheader "SaffronPCH.h"
	pchsource "Saffron/Src/SaffronPCH.cpp"

	files 
	{ 
		"%{prj.name}/Src/**.h", 
		"%{prj.name}/Src/**.c", 
		"%{prj.name}/Src/**.hpp", 
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
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


-- --------------------------------------
-- ScriptCore
-- --------------------------------------

project "ScriptCore"
	location "ScriptCore"
	kind "SharedLib"
	language "C#"

	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/Src/**.cs", 
	}
group ""


-- --------------------------------------
-- Tools
-- --------------------------------------

group "Tools"


-- --------------------------------------
-- Editor
-- --------------------------------------

project "Editor"
	location "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"
	
	targetdir ("Bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	links 
	{ 
		"Saffron"
	}
	
	files 
	{ 
		"%{prj.name}/Src/**.h", 
		"%{prj.name}/Src/**.c", 
		"%{prj.name}/Src/**.hpp", 
		"%{prj.name}/Src/**.cpp" 
	}
	
	includedirs 
	{
		"%{prj.name}/Src",
		"Saffron/Src",
		"Saffron/Vendors",
		"%{IncludeDir.entt}",
		"%{IncludeDir.FastNoise}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.spdlog}"
	}

	postbuildcommands 
	{
		'{COPY} "../Editor/Assets" "%{cfg.targetdir}/Assets"'
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
			"Saffron/%{LibraryDir.assimp_deb}"
		}

		postbuildcommands 
		{
			'{COPY} "../Saffron/Vendors/assimp/bin/Debug/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
			'{COPY} "../Saffron/Vendors/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}
				
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"

		links
		{
			"Saffron/%{LibraryDir.assimp_rel}"
		}

		postbuildcommands 
		{
			'{COPY} "../Saffron/Vendors/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
			'{COPY} "../Saffron/Vendors/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"

		links
		{
			"Saffron/%{LibraryDir.assimp_rel}"
		}

		postbuildcommands 
		{
			'{COPY} "../Saffron/Vendors/assimp/bin/Release/assimp-vc141-mtd.dll" "%{cfg.targetdir}"',
			'{COPY} "../Saffron/Vendors/mono/bin/Debug/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
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
	
	targetdir ("Editor/Assets/Scripts")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"Games/%{prj.name}/Src/**.cs", 
	}
	links
	{
		"ScriptCore"
	}
	
project "EmptyWorldProject"
	location "Games/EmptyWorldProject"
	kind "SharedLib"
	language "C#"
	
	targetdir ("Editor/Assets/Scripts")
	objdir ("Bin-Int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"Games/%{prj.name}/Src/**.cs", 
	}
	links
	{
		"ScriptCore"
	}
group ""