---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

module = {}

module.Project = "GLFW"
module.Include = function ()
	includedirs {
		GetBasePath() .. "Include"
	}
end
module.Link = function ()
	filter "configurations:Debug or Release or Dist"
		libdirs {
			GetBasePath() .. "Lib"
		}
		links {
			"glfw3_mt.lib"
		}
end

project (module.Project)
    kind "None"
    language "C++"
	
	targetdir (OutBin)
	objdir (OutObj)
	location (OutLoc)

	files {
        "Include/**.h",
		"Include/**.c",
		"Include/**.hpp",
		"Include/**.cpp",
    }

return module