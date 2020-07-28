project "ImGui"
    kind "StaticLib"
    language "C++"

	location "../%{prj.name}"
	
	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	srcDirectory = "../%{prj.name}/"
	
	targetdir ("../%{prj.name}/Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("../%{prj.name}/Bin-Int/" .. outputDirectory .. "/%{prj.name}")
	
	files
	{
        srcDirectory .. "imconfig.h",
        srcDirectory .. "imgui.h",
        srcDirectory .. "imgui.cpp",
        srcDirectory .. "imgui_draw.cpp",
        srcDirectory .. "imgui_internal.h",
        srcDirectory .. "imgui_widgets.cpp",
        srcDirectory .. "imstb_rectpack.h",
        srcDirectory .. "imstb_textedit.h",
        srcDirectory .. "imstb_truetype.h",
        srcDirectory .. "imgui_demo.cpp"
    }
    
	filter "system:windows"
        systemversion "latest"
        cppdialect "C++17"
        staticruntime "On"
        
    filter { "system:windows", "configurations:Release" }
        buildoptions "/MT"
