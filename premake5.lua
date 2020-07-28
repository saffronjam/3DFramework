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
includeDirs = {}
includeDirs["GLFW"] = "Saffron/Vendors/GLFW/include"
includeDirs["Glad"] = "Saffron/Vendors/Glad/include"
includeDirs["spdlog"] = "Saffron/Vendors/spdlog/include"
includeDirs["ImGui"] = "Saffron/Vendors/ImGui"
includeDirs["glm"] = "Saffron/Vendors/glm/include"
includeDirs["stb_image"] = "Saffron/Vendors/stb_image"

group "Dependencies"
	include "Saffron/Vendors/.Premake/GLFW"
	include "Saffron/Vendors/.Premake/Glad"
	include "Saffron/Vendors/.Premake/ImGui"
group ""

project "Saffron"
	location "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	targetdir(binDirectory .. "/" .. outputDirectory .. "/%{prj.name}")
	objdir(intDirectory .. "/" .. outputDirectory .. "/%{prj.name}")
	
	pchheader "Saffron/SaffronPCH.h"
	pchsource "Saffron/Src/Saffron/SaffronPCH.cpp"

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp",
		"%{prj.name}/Vendors/stb_image/**.h",
		"%{prj.name}/Vendors/stb_image/**.cpp",
		"%{prj.name}/Vendors/glm/glm/include/**.hpp",
		"%{prj.name}/Vendors/glm/glm/include/**.inl",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{includeDirs.GLFW}",
		"%{includeDirs.Glad}",
		"%{includeDirs.spdlog}",
		"%{includeDirs.ImGui}",
		"%{includeDirs.glm}",
		"%{includeDirs.stb_image}"
	}

	links
	{
		"ImGui",
        "GLFW",
		"GLAD",
        "opengl32.lib"
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
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		runtime "Release"
		optimize "on"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

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
		"%{includeDirs.spdlog}",
		"%{includeDirs.ImGui}",
		"%{includeDirs.glm}"
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
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		runtime "Release"
		optimize "on"