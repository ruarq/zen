workspace "zen"
	configurations { "debug", "release" }

project "zen-app"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	warnings "Extra"

	files { "src/**.hpp", "src/**.cpp" }

	buildoptions { "-mavx2" }
	linkoptions ("`sdl2-config --libs`")
	links { "fmt", "gmp", "gmpxx" }

	filter { "configurations:debug" }
		symbols "On"
		defines { "DEBUG" }

	filter { "configurations:release" }
		optimize "Speed"
