---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

module = {}

module.CopyCmd = function (from, to)
	return "{COPY} " .. from .. " " .. to
end

module.CopyAssetsToOutput = function (configuration, from, binaryOutputDir, projectDir)
    filter ("configurations:" .. configuration)    
        buildcommands {
            Utils.CopyCmd(from, binaryOutputDir),
            Utils.CopyCmd(from, projectDir)
        }
        buildoutputs { 'dummy.nonexisting' }
end

module.ProjectBin =  function (ProjectName)
    return CmnBin .. ProjectName .. "/"
end

return module