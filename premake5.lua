workspace "Saffron"
	architecture "x64"
	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
binDirectory = "Bin"
intDirectory = "Bin-Int"

includeDirs = {}
includeDirs["GLFW"] = "Saffron/Vendors/GLFW/include"
includeDirs["Glad"] = "Saffron/Vendors/Glad/include"
includeDirs["ImGui"] = "Saffron/Vendors/ImGui"

include "Saffron/Vendors/.Premake/GLFW"
include "Saffron/Vendors/.Premake/Glad"
include "Saffron/Vendors/.Premake/ImGui"

project "Saffron"
	location "Saffron"
	kind "SharedLib"
	language "C++"

	targetdir(binDirectory .. "/" .. outputDirectory .. "/%{prj.name}")
	objdir(intDirectory .. "/" .. outputDirectory .. "/%{prj.name}")

	files
	{
		"%{prj.name}/Src/**.h",
		"%{prj.name}/Src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/Src",
		"%{prj.name}/Vendors/spdlog/include",
		"%{prj.name}/Vendors/glm/include",
		"%{includeDirs.GLFW}",
		"%{includeDirs.Glad}",
		"%{includeDirs.ImGui}"
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
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"SE_PLATFORM_WINDOWS",
			"SE_BUILD_DLL",
			"GLFW_INCLUDE_NONE"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../" .. binDirectory .."/" ..outputDirectory.. "/Sandbox")
		}

	filter "configurations:Debug"
		defines
		{
			"SE_DEBUG",
			"SE_ENABLE_ASSERTS"
		}
		symbols "On"

	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"



project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

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
		"Saffron/Vendors/spdlog/include",
		"Saffron/Vendors/glm/include"
	}

	links
	{
		"Saffron"
	}

	disablewarnings
	{
		"4251"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "on"
		systemversion "latest"

		defines
		{
			"SE_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines
		{
			"SE_DEBUG",
			"SE_ENABLE_ASSERTS"
		}
		symbols "On"

	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"