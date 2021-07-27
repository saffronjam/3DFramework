---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

project (ProjectName)
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
	staticruntime "On"

	objdir (OutObj)
	location (OutLoc)

    filter "configurations:Debug or Release"
	    targetdir (OutBin)
        Engine.PreBuild("Debug", OutBin, PrjLoc)
        Engine.PostBuild(ProjectName, "Debug", OutBin, PrjLoc)
        Engine.PreBuild("Release", OutBin, PrjLoc)
        Engine.PostBuild(ProjectName, "Release", OutBin, PrjLoc)

    filter "configurations:Dist"
        targetdir (OutBinDist)
        Engine.PreBuild("Dist", OutBinDist, PrjLoc)
        Engine.PostBuild(ProjectName, "Dist", OutBinDist, PrjLoc)

    filter "configurations:Debug or Release or Dist"

    files {
        "Source/**.h",
		"Source/**.c",
		"Source/**.hpp",
		"Source/**.cpp",
		"Assets/Shaders/**.*",
    }

    vpaths {
        ["Shaders"] = { "Assets/Shaders/**.*" }
    }

    includedirs {
        "Source",
    }
    
    Engine.Include()
    Engine.Link()
    Engine.AddDefines()

    local from = GetBasePath() .. AstFol
    Utils.CopyAssetsToOutput("Debug", from, OutBin .. AstFol, PrjLoc .. AstFol)
    Utils.CopyAssetsToOutput("Release", from, OutBin  .. AstFol, PrjLoc .. AstFol)
    Utils.CopyAssetsToOutput("Dist", from, OutBinDist  .. AstFol, PrjLoc .. AstFol)

    filter "configurations:Debug"
        symbols "On"

    filter "configurations:Release"
        optimize "On"

    filter "configurations:Dist"
        optimize "On"
