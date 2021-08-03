---@diagnostic disable: undefined-global, undefined-field

Inspect = require('inspect')
Utils = require('utils')

CfgSys     = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}/"
CmnBin     = _MAIN_SCRIPT_DIR .. "/Build/Bin/" .. CfgSys
CmnObj     = _MAIN_SCRIPT_DIR .. "/Build/Obj/" .. CfgSys
OutBin     = CmnBin .. "%{prj.name}/"
OutObj     = CmnObj .. "%{prj.name}/"
OutBinDist = _MAIN_SCRIPT_DIR .. "/Example/%{cfg.system}/"
OutLoc     = _MAIN_SCRIPT_DIR .. "/Build/"
PrjLoc     = _MAIN_SCRIPT_DIR .. "/Build/"
WrkLoc     = _MAIN_SCRIPT_DIR .. "/"
AstFol     = "Assets/"

local function GetBasePath()
	return debug.getinfo(1).source:match("@?(.*/)")
end

local function RequireAll()
    local result = {}
    result["DirectX"] = require("ThirdParty.DirectX.premake5")
    result["DirectXTK"] = require("ThirdParty.DirectXTK.premake5")
    result["ImGui"] = require("ThirdParty.imgui.premake5")
    result["Assimp"] = require("ThirdParty.assimp.premake5")
    return result
end

local ThirdParties

local function LinkAll()
    for k, v in pairs(ThirdParties) do
        if v.Link ~= nil then
            v.Link()
        end
    end
end

local function IncludeAll()
    for k, v in pairs(ThirdParties) do
        if v.Include ~= nil then
            v.Include()
        end
    end
end

local function PreBuildAll(Configuration, BinaryOutputDir, ProjectDir)
    for k, v in pairs(ThirdParties) do
        if v.PreBuild ~= nil then
            v.PreBuild(Configuration, BinaryOutputDir, ProjectDir)            
        end
    end
end

local function PostBuildAll(Configuration, BinaryOutputDir, ProjectDir)
    for k, v in pairs(ThirdParties) do
        if v.PostBuild ~= nil then
            v.PostBuild(Configuration, BinaryOutputDir, ProjectDir)            
        end
    end
end


-- Start module

local module = {}

module.Project = "SaffronEngine2D"

module.Include = function ()
    includedirs {
        GetBasePath() .. "Source"
    }
    -- Include third parties
    IncludeAll()
end

module.Link = function ()
    links {
        module.Project
    }
end

module.PreBuild = function (Configuration, BinaryOutputDir, ProjectDir)
    disablewarnings {
        "4244",
        "4267"
    }
    PreBuildAll(Configuration, BinaryOutputDir, ProjectDir)
end

module.PostBuild = function (Project, Configuration, BinaryOutputDir, ProjectDir)
    local resFrom = GetBasePath() .. AstFol
    local shrBinFrom = Utils.ProjectBin(module.Project) .. "Shaders/"

    local resBinTo = BinaryOutputDir .. AstFol
    local resPrjTo = ProjectDir .. AstFol
    local shrBinTo = resBinTo .. "Shaders/"
    local shrPrjTo = resPrjTo .. "Shaders/"

    Utils.CopyAssetsToOutput(Configuration, resFrom, resBinTo, resPrjTo);
    Utils.CopyAssetsToOutput(Configuration, shrBinFrom, shrBinTo, shrPrjTo);

    PostBuildAll(Configuration, BinaryOutputDir, ProjectDir)
end

module.SetStartUpProject = function (projectName)
    workspace "Saffron"
        startproject (projectName)
end


group "Engine/ThirdParty"
ThirdParties = RequireAll()
group "Engine"

module.AddDefines = function()
	filter "configurations:Debug or Release or Dist"
        defines {
            "SE_IMGUI_INI_PATH=\"Assets/imgui.ini\""
        }
    filter "system:windows"
        defines {
            "_CRT_SECURE_NO_WARNINGS",
            "SE_PLATFORM_WINDOWS",
        }
    filter "configurations:Debug"
        defines { "DEBUG", "SE_DEBUG" }
    filter "configurations:Release"
        defines { "NDEBUG", "SE_RELEASE" }
    filter "configurations:Dist"
        defines { "NDEBUG", "SE_DIST" }
end

project (module.Project)
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
	staticruntime "On"

	pchheader ("SaffronPCH.h")
	pchsource ("Source/SaffronPCH.cpp")

	targetdir (OutBin)
	objdir (OutObj)
	location (OutLoc)

    files {
        "Source/**.h",
		"Source/**.c",
		"Source/**.hpp",
		"Source/**.cpp",
		"Assets/Shaders/**.hlsl",
    }

    vpaths {
        ["Engine/Shaders"] = { "Assets/Shaders/**.*" }
    }

    disablewarnings {
        "4244",
        "4267"
    }

    -- Include self and third parties
    module.Include()

    -- Link third parties
    LinkAll()

    module.AddDefines()

    filter "configurations:Debug"
        symbols "On"
    filter "configurations:Release"
        optimize "On"
    filter "configurations:Dist"
        optimize "On"


    filter { "files:**_p.hlsl" }
        shadermodel "5.0"
        shadertype "Pixel"
        shaderobjectfileoutput(OutBin .. "Shaders/%{file.basename}.cso")

    filter { "files:**_v.hlsl" }
        shadermodel "5.0"
        shadertype "Vertex"
        shaderobjectfileoutput(OutBin .. "Shaders/%{file.basename}.cso")

group ""

return module