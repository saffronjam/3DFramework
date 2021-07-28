---@diagnostic disable: undefined-global

module = {}

module.Project = "DirectX"
module.Link = function ()
	filter "configurations:Debug"
		links {
			"D3D11.lib",
			"dxguid.lib",
			"dxgi.lib"
		}

	filter "configurations:Release or Dist"
		links {
			"D3D11.lib",
			"dxgi.lib",
		}
end

project (module.Project)
    kind "None"
    language "C++"
	
	targetdir (OutBin)
	objdir (OutObj)
	location (OutLoc)

return module