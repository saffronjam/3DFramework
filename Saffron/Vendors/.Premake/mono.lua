project "mono"
    kind "None"

	location "../%{prj.name}"
	objdir ("../%{prj.name}/Unused/")
	
	srcDirectory = "../%{prj.name}/"
	
    files
    {
        srcDirectory .. "include/mono/**.*",
    }

    includedirs
    {
        srcDirectory .. "include"
    }