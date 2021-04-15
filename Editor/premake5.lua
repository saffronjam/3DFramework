project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"	
	
	targetdir ("../Bin/" .. outputDirectory .. "/Editor")
	objdir ("../Bin-Int/" .. outputDirectory .. "/Editor")
	

	-- pchheader "../Engine/Source/SaffronPCH.h"
	-- pchsource "../Engine/Source/SaffronPCH.cpp"
	
	local EngineLocation = "../Engine/"
	local ThirdPartyLocation = EngineLocation .. "ThirdParty/"
	
	files 
	{ 
		"Source/**.h", 
		"Source/**.c", 
		"Source/**.hpp", 
		"Source/**.cpp",
	}
	
	includedirs
	{
		"Source",
		EngineLocation .. "Source",
		ThirdPartyLocation .. "assimp/include",
		ThirdPartyLocation .. "Box2D/include",
		ThirdPartyLocation .. "entt/include",
		ThirdPartyLocation .. "FastNoise/include",
		ThirdPartyLocation .. "Glad/include",
		ThirdPartyLocation .. "GLFW/include",
		ThirdPartyLocation .. "glm/include",
		ThirdPartyLocation .. "imgui",
		ThirdPartyLocation .. "mono/include",
		ThirdPartyLocation .. "reactphysics3d/include",
		ThirdPartyLocation .. "spdlog/include",
		ThirdPartyLocation .. "stb/include",
		ThirdPartyLocation .. "yaml-cpp/include",
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