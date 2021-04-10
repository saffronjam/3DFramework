project "DirectXTK"
    kind "None"
	
    files
    {
        "include/%{prj.name}/**.h"
    }

    includedirs
    {
        "include"
    }
	
	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbolspath "lib/Debug/DirectXTK.pdb"
		symbols "On"
		
		links
		{
			"%{PrebuiltLibDeb.DirectXTK}",
			"%{PrebuiltLibDeb.DirectXTex}"
		}
			
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"
		
		links
		{
			"%{PrebuiltLibRel.DirectXTK}"
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"
		
		links
		{
			"%{PrebuiltLibRel.DirectXTK}",
			"%{PrebuiltLibRel.DirectXTex}"
		}