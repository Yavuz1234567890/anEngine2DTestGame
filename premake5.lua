workspace "anEngine2D"
	location ""
	architecture "x86_64"
	startproject "anEngine2DTestGame"

	configurations
	{
		"Debug",
		"Release"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"thirdparty/glfw/include",
		"thirdparty/glew/include",
		"thirdparty/freetype/include",
		"thirdparty/stb",
		"src/anEngine2D"
	}

	filter "system:windows"
		defines
		{
			"PLATFORM_WINDOWS"
		}

	project "anEngine2D"
		location "src/anEngine2D"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin"
		objdir "obj"

		files
		{
			"src/anEngine2D/**.cpp",
			"src/anEngine2D/**.h",
			
			"thirdparty/stb/**.cpp",
			"thirdparty/stb/**.h"
		}

		filter "system:windows"
			systemversion "latest"

			libdirs
			{
				"thirdparty/glew/lib-x64",
				"thirdparty/glfw/lib-x64",
				"thirdparty/freetype/lib-x64"
			}

			links
			{
				"opengl32.lib",
				"glew32.lib",
				"glfw3.lib",
				"freetype.lib"
			}

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"

	project "anEngine2DTestGame"
		location "src/anEngine2DTestGame"
		kind "WindowedApp"
		language "C++"
		cppdialect "C++17"
		staticruntime "off"

		targetdir "bin"
		objdir "obj"

		files
		{
			"src/anEngine2DTestGame/**.cpp",
			"src/anEngine2DTestGame/**.h"
		}

		links
		{
			"anEngine2D"
		}

		filter "system:windows"
			systemversion "latest"

		filter "configurations:Debug"
			defines "C_DEBUG"
			runtime "Debug"
			symbols "on"

		filter "configurations:Release"
			defines "C_RELEASE"
			runtime "Release"
			optimize "on"
