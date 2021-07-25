---@diagnostic disable: undefined-global

ProjectName = "Editor"

workspace "Saffron"
	architecture "x64"

	configurations {
		"Debug",
		"Release",
		"Dist"
	}

	flags {
		"MultiProcessorCompile"
	}

    location (WrkLoc)

	startproject (ProjectName)

Engine = require("Engine.premake5")
Editor = require("Editor.premake5")