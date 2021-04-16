#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui_internal.h"
#include "core/Renderer.h"
#include <vector>

int main(int, char**)
{
	Renderer renderer(1280, 720);
	renderer.Initialize();

	while (!glfwWindowShouldClose(renderer.GetWindow()))
	{
		renderer.Update();
	}


	return 0;
}