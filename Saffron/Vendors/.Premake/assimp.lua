project "assimp"
    kind "None"

	location "../%{prj.name}"
	objdir ("../%{prj.name}/Unused/")
	
	srcDirectory = "../%{prj.name}/"
	
    files
    {
        srcDirectory .. "include/assimp/**.h",
        srcDirectory .. "include/assimp/**.hpp",
    }

    includedirs
    {
        srcDirectory .. "include"
    }