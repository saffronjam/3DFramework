---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

module = {}

module.Project = "Assimp"
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
			"assimp-vc142-mtd.lib"
		}

	filter "configurations:Release or Dist"
		libdirs {
			GetBasePath() .. "Lib/Release"
		}
		links {
			"assimp-vc142-mt.lib"
		}
end

module.PostBuild = function(Configuration, BinaryOutputDir, ProjectDir)
	filter ("configurations:" .. Configuration)
		local from = ""
		if Configuration == "Debug" then
			from = GetBasePath() .. "Bin/Debug/"
		elseif Configuration == "Release" or Configuration == "Dist" then
			from = GetBasePath() .. "Bin/Release/"
		end
	
		if from ~= "" then
			buildcommands {
				Utils.CopyCmd(from, BinaryOutputDir),
				Utils.CopyCmd(from, ProjectDir)
			}
			buildoutputs { 'dummy.nonexisting' }
		end
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