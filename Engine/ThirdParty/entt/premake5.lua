---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

module = {}

module.Project = "entt"
module.Include = function ()
	includedirs {
		GetBasePath() .. "Include"
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