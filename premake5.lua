workspace "Saffron"
	architecture "x64"
	startproject "Sandbox"

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

outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
binDirectory = "Bin"
intDirectory = "Bin-Int"

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
LibraryDirs["mono"] = "Saffron/Vendors/mono/lib/Debug/mono-2.0-sgen.lib"

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

project "Saffron"
	location "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir(binDirectory .. "/" .. outputDirectory .. "/%{prj.name}")
	objdir(intDirectory .. "/" .. outputDirectory .. "/%{prj.name}")
	
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
		"${LibraryDirs.mono}"
	}

	disablewarnings
	{
		"4251"
	}
	
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		runtime "Release"
		optimize "On"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir(binDirectory .. "/" .. outputDirectory .. "/%{prj.name}")
	objdir(intDirectory .. "/" .. outputDirectory .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"Saffron/Src",
		"%{IncludeDirs.spdlog}",
		"%{IncludeDirs.ImGui}",
		"%{IncludeDirs.glm}",
		"%{IncludeDirs.entt}"
	}

	links
	{
		"Saffron",
		"ImGui"
	}

	disablewarnings
	{
		"4251"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "SE_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		runtime "Release"
		optimize "On"