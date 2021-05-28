project "ScriptGenerator"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"

	targetdir ("../Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("../Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	files 
	{
		"Source/**.cpp", 
		"Source/**.h", 
	}
group ""