project "ScriptCore"
	kind "SharedLib"
	language "C#"

	targetdir ("../Bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("../Bin-Int/" .. outputDirectory .. "/%{prj.name}")

	files 
	{
		"Source/**.cs", 
	}
group ""