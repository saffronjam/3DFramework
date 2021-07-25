---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

module = {}

module.CopyCmd = function (from, to)
	return "{COPY} " .. from .. " " .. to
end

function CopyAssetsToOutput (configuration, from, binaryOutputDir, projectDir)
    filter ("configurations:" .. configuration)
        postbuildcommands {
            Utils.CopyCmd(from, binaryOutputDir),
            Utils.CopyCmd(from, projectDir)
        }
end

return module