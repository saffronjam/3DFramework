package.path = package.path .. ";../common.lua"
common = require("common")

project "2DGameProject"
	kind "SharedLib"
	language "C#"
	
	targetdir (common.OutputDir)
	objdir (common.ObjDir)

	files 
	{
		"Source/**.cs", 
	}
	links
	{
		"ScriptCore"
	}