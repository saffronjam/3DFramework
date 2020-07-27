project "GLFW"
	kind "StaticLib"
	language "C"

	location "../%{prj.name}"
	
	outputDirectory = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
	srcDirectory = "../%{prj.name}/"
	
	targetdir ("bin/" .. outputDirectory .. "/%{prj.name}")
	objdir ("bin-int/" .. outputDirectory .. "/%{prj.name}")
	

	files
	{
		srcDirectory .. "include/GLFW/glfw3.h",
		srcDirectory .. "include/GLFW/glfw3native.h",
		srcDirectory .. "src/glfw_config.h",
		srcDirectory .. "src/context.c",
		srcDirectory .. "src/init.c",
		srcDirectory .. "src/input.c",
		srcDirectory .. "src/monitor.c",
		srcDirectory .. "src/vulkan.c",
		srcDirectory .. "src/window.c"
	}
	filter "system:linux"
		pic "On"

		systemversion "latest"
		staticruntime "On"

		files
		{
			srcDirectory .. "src/x11_init.c",
			srcDirectory .. "src/x11_monitor.c",
			srcDirectory .. "src/x11_window.c",
			srcDirectory .. "src/xkb_unicode.c",
			srcDirectory .. "src/posix_time.c",
			srcDirectory .. "src/posix_thread.c",
			srcDirectory .. "src/glx_context.c",
			srcDirectory .. "src/egl_context.c",
			srcDirectory .. "src/osmesa_context.c",
			srcDirectory .. "src/linux_joystick.c"
		}

		defines
		{
			"_GLFW_X11"
		}

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"

		files
		{
			srcDirectory .. "src/win32_init.c",
			srcDirectory .. "src/win32_joystick.c",
			srcDirectory .. "src/win32_monitor.c",
			srcDirectory .. "src/win32_time.c",
			srcDirectory .. "src/win32_thread.c",
			srcDirectory .. "src/win32_window.c",
			srcDirectory .. "src/wgl_context.c",
			srcDirectory .. "src/egl_context.c",
			srcDirectory .. "src/osmesa_context.c"
		}

		defines 
		{ 
			"_GLFW_WIN32",
			"_CRT_SECURE_NO_WARNINGS"
		}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "on"
