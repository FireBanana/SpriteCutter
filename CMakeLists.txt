cmake_minimum_required(VERSION 3.10.2)

project(SpriteCutter)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_SUPPRESS_REGENERATION true)

add_executable(SpriteCutter "SpriteCutter/main.cpp")
target_include_directories(SpriteCutter PUBLIC "SpriteCutter/")

target_sources(SpriteCutter
	PRIVATE
	"SpriteCutter/glad.c"
	"SpriteCutter/imgui_impl_glfw.cpp"
	"SpriteCutter/imgui_impl_glfw.h"
	"SpriteCutter/imgui_impl_opengl3.cpp"
	"SpriteCutter/imgui_impl_opengl3.h"
	"SpriteCutter/stb_image.h"
)