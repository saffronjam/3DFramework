workspace "Saffron"
	architecture "x64"
	targetdir "build"

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

	startproject "Sandbox"
	
OutputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
BinDirectory = "Bin"
IntDirectory = "Bin-Int"

-- Include directories relative to root folder (solution directory)
IncludeDirs = {}
IncludeDirs["assimp"] = "Saffron/Vendors/assimp/include"
IncludeDirs["Box2D"] = "Saffron/Vendors/Box2D/include"
IncludeDirs["entt"] = "Saffron/Vendors/entt/include"
IncludeDirs["Glad"] = "Saffron/Vendors/Glad/include"
IncludeDirs["GLFW"] = "Saffron/Vendors/GLFW/include"
IncludeDirs["glm"] = "Saffron/Vendors/glm/include"
IncludeDirs["ImGui"] = "Saffron/Vendors/ImGui"
IncludeDirs["mono"] = "Saffron/Vendors/mono/include"
IncludeDirs["spdlog"] = "Saffron/Vendors/spdlog/include"
IncludeDirs["stb"] = "Saffron/Vendors/stb/include"
IncludeDirs["yamlcpp"] = "Saffron/Vendors/yaml-cpp/include"

-- Library directories relative to root folder (solution directory)
LibraryDirs = {}
LibraryDirs["mono"] = "Vendors/mono/lib/Debug/mono-2.0-sgen.lib"
LibraryDirs["assimp_deb"] = "Vendors/assimp/lib/Debug/assimp-vc141-mtd.lib"
LibraryDirs["assimp_rel"] = "Vendors/assimp/lib/Release/assimp-vc141-mt.lib"

group "Vendors"
	include "Saffron/Vendors/.Premake/assimp"
	include "Saffron/Vendors/.Premake/Box2D"
	include "Saffron/Vendors/.Premake/entt"
	include "Saffron/Vendors/.Premake/Glad"
	include "Saffron/Vendors/.Premake/GLFW"
	include "Saffron/Vendors/.Premake/glm"
	include "Saffron/Vendors/.Premake/ImGui"
	include "Saffron/Vendors/.Premake/mono"
	include "Saffron/Vendors/.Premake/stb"
	include "Saffron/Vendors/.Premake/yaml-cpp"
group ""


-- --------------------------------------
-- Core
-- --------------------------------------

group "Core"


-- --------------------------------------
-- Saffron
-- --------------------------------------

project "Saffron"
	location "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir(BinDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")
	objdir(IntDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")
	
	pchheader "Saffron/SaffronPCH.h"
	pchsource "Saffron/Src/Saffron/SaffronPCH.cpp"

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{IncludeDirs.assimp}",
		"%{IncludeDirs.Box2D}",
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.GLFW}",
		"%{IncludeDirs.Glad}",
		"%{IncludeDirs.mono}",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.stb}",
		"%{IncludeDirs.yamlcpp}"
	}

	links
	{
		"Box2D",
		"Glad",
        "GLFW",
		"ImGui",
		"opengl32.lib",
		"%{LibraryDirs.mono}",
		"yaml-cpp"
	}

	disablewarnings
	{
		"4251"
	}
	
	filter "system:windows"
		systemversion "latest"
		defines "GLFW_INCLUDE_NONE"

	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
		
		links
		{
			"%{LibraryDirs.assimp_deb}"
		}
		
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"
		
		links
		{
			"%{LibraryDirs.assimp_rel}"
		}
		
	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"
		
		links
		{
			"%{LibraryDirs.assimp_rel}"
		}
group ""



-- --------------------------------------
-- TOOLS
-- --------------------------------------

group "Tools"


-- --------------------------------------
-- SaffronBun
-- --------------------------------------

project "SaffronBun"
	location "SaffronBun"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir(BinDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")
	objdir(IntDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")

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
		"%{IncludeDirs.entt}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.spdlog}"
	}
	
	postbuildcommands 
	{
		'{COPY} "../SaffronBun/Assets" "%{cfg.targetdir}/Assets"'
	}

	filter "system:windows"
		systemversion "latest"

		defines 
		{ 
			"SE_PLATFORM_WINDOWS"
		}
		
	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
		
		links
		{
			"Saffron/%{LibraryDirs.assimp_deb}"
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
			"Saffron/%{LibraryDirs.assimp_rel}"
		}
		
		postbuildcommands 
		{
			'{COPY} "../Saffron/Vendors/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
			'{COPY} "../Saffron/Vendors/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"
		
		links
		{
			"Saffron/%{LibraryDirs.assimp_rel}"
		}
		
		postbuildcommands 
		{
			'{COPY} "../Saffron/Vendors/assimp/bin/Release/assimp-vc141-mt.dll" "%{cfg.targetdir}"',
			'{COPY} "../Saffron/Vendors/mono/bin/Release/mono-2.0-sgen.dll" "%{cfg.targetdir}"'
		}
group ""


-- --------------------------------------
-- Sandbox
-- --------------------------------------

group "Sandbox"
-- workspace "Sandbox"
	-- architecture "x64"
	-- targetdir "build"
	
	-- configurations 
	-- { 
		-- "Debug", 
		-- "Release",
		-- "Dist"
	-- }


-- --------------------------------------
-- Saffron ScriptCore
-- --------------------------------------

project "Saffron-ScriptCore"
	location "Saffron-ScriptCore"
	kind "SharedLib"
	language "C#"

	targetdir(BinDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")
	objdir(IntDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/Src/**.cs", 
	}
	
	filter "configurations:Debug"
		symbols "On"


-- --------------------------------------
-- ExampleApp
-- --------------------------------------

project "ExampleApp"
	location "ExampleApp"
	kind "SharedLib"
	language "C#"

	targetdir(BinDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")
	objdir(IntDirectory .. "/" .. OutputDirectory .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/Src/**.cs", 
	}

	links
	{
		"Saffron-ScriptCore"
	}
	
	filter "configurations:Debug"
		symbols "On"
group ""


