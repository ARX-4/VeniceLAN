workspace "VeniceLAN"
	configurations { "Debug", "Release" }

	symbols "On"
	characterset "Unicode"

	flags { "StaticRuntime", "No64BitChecks" }

	flags { "NoIncrementalLink", "NoEditAndContinue" }

	location "build/"

	architecture "x86"

    filter "configurations:Debug"
        defines "_DEBUG"

    filter "configurations:Release"
        defines "NDEBUG"
        optimize "Speed"

    targetdir "bin/%{cfg.buildcfg}/"
	objdir "obj/%{cfg.buildcfg}/"

	buildoptions "/MP"

project "VeniceLAN"
	targetname "dinput8"
	language "C++"
	kind "SharedLib"

	includedirs { "include/", "include/BlazeLoop/", "vendor/jitasm/" }

	links { "ws2_32" }

	files
	{
		"include/**.h",
		"src/**.cpp",
		"src/VeniceLAN.def"
	}

	pchsource "src/StdInc.cpp"
	pchheader "StdInc.h"

	postbuildcommands { "if exist \"$(SolutionDir)..\\postbuild.cmd\" call \"$(SolutionDir)..\\postbuild.cmd\" \"$(TargetPath)\" \"$(TargetFileName)\" %{prj.name}" }