-- ----------------------------------------------------
-- ----------------------------------------------------
-- --------- Saffron Engine Project Generator ---------
-- ----------------------------------------------------
-- ----------------------------------------------------


-- --------------------------------------
-- Saffron Workspace
-- --------------------------------------

workspace "Saffron"
	architecture "x64"
	
	configurations 
	{ 
		"Debug", 
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	startproject "Editor"

outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- --------------------------------------
-- Engine
-- --------------------------------------

group "Engine"

group "Engine/Dependencies"
	-- include "Engine/ThirdParty/assimp/premake5"
	include "Engine/ThirdParty/Box2D/premake5"
	-- include "Engine/ThirdParty/entt/premake5"
	include "Engine/ThirdParty/FastNoise/premake5"
	include "Engine/ThirdParty/Glad/premake5"
	include "Engine/ThirdParty/GLFW/premake5"
	-- include "Engine/ThirdParty/glm/premake5"
	include "Engine/ThirdParty/ImGui/premake5"
	include "Engine/ThirdParty/mono/premake5"
	-- include "Engine/ThirdParty/spdlog/premake5"
	-- include "Engine/ThirdParty/stb/premake5"
	include "Engine/ThirdParty/yaml-cpp/premake5"
group "Engine"
	include "Engine/premake5"
	include "ScriptCore/premake5"
group "Games"
	include "Games/2DGameProject/premake5"
group ""

include "Editor/premake5"





