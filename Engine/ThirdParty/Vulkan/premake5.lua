---@diagnostic disable: undefined-global

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

vkPath = 'C:/VulkanSDK/1.3.204.1/'

module = {}

module.Project = "Vulkan"
module.Include = function ()
	includedirs {
		vkPath .. "Include"
	}
end
module.Link = function ()
    libdirs {
        vkPath .. "Lib"
    }
    links {
        "vulkan-1.lib"
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