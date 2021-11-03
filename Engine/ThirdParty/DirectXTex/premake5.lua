---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

module = {}

module.Project = "DirectXTex"
module.Include = function ()
	includedirs {
		GetBasePath() .. "Include"
	}
end
module.Link = function ()
	filter "configurations:Debug"
		libdirs {
			GetBasePath() .. "Lib/Debug"
		}
		links {
			"DirectXTex.lib"
		}

		print(GetBasePath() .. "Lib/Release")

	filter "configurations:Release or Dist"
		libdirs {
			GetBasePath() .. "Lib/Release"
		}
		links {
			"DirectXTex.lib"
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