workspace "zen"
	configurations { "debug", "release" }

project "zen-app"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	warnings "Extra"

	files { "src/**.hpp", "src/**.cpp" }

	links { "fmt", "gmp", "gmpxx", "SDL2" }

	filter { "configurations:debug" }
		symbols "On"
		defines { "DEBUG" }

	filter { "configurations:release" }
		optimize "Speed"
