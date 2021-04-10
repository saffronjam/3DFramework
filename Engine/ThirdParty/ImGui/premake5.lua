project "ImGui"
    kind "StaticLib"
    language "C++"
    staticruntime "on"
	
	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	
	targetdir ("Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
	includedirs
    {
        "include/ImGui"
    }
	
	files
	{
		"src/**.h",
		"src/**.cpp",
		
	
        -- "src/imconfig.h",
        -- "src/imgui.h",
        -- "src/imgui.cpp",
        -- "src/imgui_draw.cpp",
        -- "src/imgui_internal.h",
        -- "src/imgui_widgets.cpp",
        -- "src/imstb_rectpack.h",
        -- "src/imstb_textedit.h",
        -- "src/imstb_truetype.h",
        -- "src/imgui_demo.cpp",
        -- "src/imgui_impl_dx11.cpp",
        -- "src/imgui_impl_win32.cpp",
    }
    
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		runtime "Release"
		optimize "on"