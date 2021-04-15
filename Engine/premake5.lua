-- --------------------------------------
-- Saffron
-- --------------------------------------

LibDeb = {}
LibDeb["assimp"] 		= "ThirdParty/assimp/lib/Debug/assimp-vc141-mtd.lib"
LibDeb["mono"] 			= "ThirdParty/mono/lib/Debug/mono-2.0-sgen.lib"

LibRel = {}
LibRel["assimp"] 		= "ThirdParty/assimp/lib/Release/assimp-vc141-mt.lib"
LibRel["mono"] 			= "ThirdParty/mono/lib/Debug/mono-2.0-sgen.lib"

SharedLibDeb = {}
SharedLibDeb["assimp"] 	= "ThirdParty/assimp/bin/Debug/assimp-vc141-mtd.dll"
SharedLibDeb["mono"] 		= "ThirdParty/mono/bin/Debug/mono-2.0-sgen.dll"

SharedLibRel = {}
SharedLibRel["assimp"] 	= "ThirdParty/assimp/bin/Release/assimp-vc141-mt.dll"
SharedLibDeb["mono"] 	= "ThirdParty/mono/bin/Debug/mono-2.0-sgen.dll"

RuntimeLibDir = {}
RuntimeLibDir["mono"] = "ThirdParty/mono/bin/Runtime"

project "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("../Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("../Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	pchheader "SaffronPCH.h"
	pchsource "Source/SaffronPCH.cpp"

	EditorTargetDir = "../Bin/" .. outputDirectory .. "/Editor"

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
		"ThirdParty/assimp/include",
		"ThirdParty/Box2D/include",
		"ThirdParty/entt/include",
		"ThirdParty/FastNoise/include",
		"ThirdParty/Glad/include",
		"ThirdParty/GLFW/include",
		"ThirdParty/glm/include",
		"ThirdParty/imgui",
		"ThirdParty/mono/include",
		"ThirdParty/reactphysics3d/include",
		"ThirdParty/spdlog/include",
		"ThirdParty/stb/include",
		"ThirdParty/yaml-cpp/include",
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
	}
	
	postbuildcommands 
	{
		-- '{COPY} "Assets" "%{cfg.targetdir}/Assets"',
		'{COPY} "%{RuntimeLibDir.mono}" "../Editor/Runtime"',
	}
		
	filter "system:windows"
		systemversion "latest"
		
		defines 
		{ 
			"_CRT_SECURE_NO_WARNINGS",
			"SE_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
		
		links
		{
			"%{LibDeb.assimp}",
			"%{LibDeb.mono}",
		}
		
		postbuildcommands 
		{
			'{COPY} "%{SharedLibDeb.assimp}" "%{EditorTargetDir}"',
			'{COPY} "%{SharedLibDeb.mono}" "%{EditorTargetDir}"',
		}
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"
		
		links
		{
			"%{LibRel.assimp}",
			"%{LibRel.mono}",
		}
		
		postbuildcommands 
		{
			'{COPY} "%{SharedLibRel.assimp}" "%{EditorTargetDir}"',
			'{COPY} "%{SharedLibRel.mono}" "%{EditorTargetDir}"',
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"
		
		links
		{
			"%{LibRel.assimp}",
			"%{LibRel.mono}",
		}
		
		postbuildcommands 
		{
			'{COPY} "%{SharedLibRel.assimp_rel}" "%{EditorTargetDir}"',
			'{COPY} "%{SharedLibRel.mono}" "%{EditorTargetDir}"',
		}
		