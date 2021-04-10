-- --------------------------------------
-- Saffron
-- --------------------------------------

LibraryDir = {}

PrebuiltLibDeb = {}
PrebuiltLibDeb["bgfx"] = "ThirdParty/bgfx/lib/bgfxDebug.lib"
PrebuiltLibDeb["bimg"] = "ThirdParty/bgfx/lib/bimgDebug.lib"
PrebuiltLibDeb["bx"] = "ThirdParty/bgfx/lib/bxDebug.lib"
PrebuiltLibDeb["DirectXTK"] = "ThirdParty/DirectXTK/lib/Debug/DirectXTK.lib"

PrebuiltLibRel = {}
PrebuiltLibRel["bgfx"] = "ThirdParty/bgfx/lib/bgfxRelease.lib"
PrebuiltLibRel["bimg"] = "ThirdParty/bgfx/lib/bimgRelease.lib"
PrebuiltLibRel["bx"] = "ThirdParty/bgfx/lib/bxRelease.lib"
PrebuiltLibRel["DirectXTK"] = "ThirdParty/DirectXTK/lib/Release/DirectXTK.lib"


project "Saffron"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "On"

	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	pchheader "SaffronPCH.h"
	pchsource "Source/SaffronPCH.cpp"

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
		
		"ThirdParty/bgfx/include",
		"ThirdParty/DirectXTK/include",
		"ThirdParty/ImGui/include",
		"ThirdParty/Spdlog/include",
	}
	
	links 
	{ 
		"ImGui",
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
			"%{PrebuiltLibDeb.bgfx}",
			"%{PrebuiltLibDeb.bimg}",
			"%{PrebuiltLibDeb.bx}",
			"%{PrebuiltLibDeb.DirectXTK}",
		}
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"
		links
		{			
			"%{PrebuiltLibRel.bgfx}",
			"%{PrebuiltLibRel.bimg}",
			"%{PrebuiltLibRel.bx}",
			"%{PrebuiltLibRel.DirectXTK}",
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"		
		links
		{			
			"%{PrebuiltLibRel.bgfx}",
			"%{PrebuiltLibRel.bimg}",
			"%{PrebuiltLibRel.bx}",
			"%{PrebuiltLibDeb.DirectXTK}",
		}	