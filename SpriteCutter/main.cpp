// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#define STB_IMAGE_IMPLEMENTATION
#include <Windows.h>
#include <commdlg.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"
#include "imgui_internal.h"
#include "core/TransformableImage.h"
#include "core/GridViewer.h"
#include <vector>

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

int main(int, char**)
{
	if (!glfwInit())
		return 1;

	GLFWwindow* window = glfwCreateWindow(1280, 720, "Sprite Cutter", NULL, NULL);
	if (window == NULL)
		return 1;

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);

	//
	GLuint texId = 0;
	TransformableImage* image = nullptr;
	GridViewer* grid = nullptr;
	int image_width = 0, image_height = 0;
	int density = 16;

	std::vector<std::string> collection_list;
	int selection = 0;

	while (!glfwWindowShouldClose(window))
	{
		int display_w, display_h;
		glfwGetFramebufferSize(window, &display_w, &display_h);

		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::DockSpaceOverViewport();

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Import"))
				{
					OPENFILENAME ofn;       // common dialog box structure
					char szFile[260];             // owner window
					HANDLE hf;              // file handle

					// Initialize OPENFILENAME
					ZeroMemory(&ofn, sizeof(ofn));
					ofn.lStructSize = sizeof(ofn);
					ofn.hwndOwner = NULL;
					ofn.lpstrFile = szFile;
					ofn.lpstrFile[0] = '\0';
					ofn.nMaxFile = sizeof(szFile);
					ofn.nFilterIndex = 1;
					ofn.lpstrFileTitle = NULL;
					ofn.nMaxFileTitle = 0;
					ofn.lpstrInitialDir = NULL;
					ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

					// Display the Open dialog box. 

					if (GetOpenFileName(&ofn) == TRUE)
					{
						delete image;
						image = nullptr;

						delete grid;
						grid = nullptr;

						LoadTextureFromFile(ofn.lpstrFile, &texId, &image_width, &image_height);
					}
				}
				ImGui::EndMenu();
			}

			if (texId)
			{

				ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 175, 0));
				ImGui::PushItemWidth(50);
				if (ImGui::DragInt("Division Size", &density, 0.05f, 16, 1000))
				{
					grid->ScaleTiles();
				}
			}


			ImGui::EndMainMenuBar();
		}
		ImGui::PopStyleVar();

		ImGui::Begin("Sprite Tool");
		if (texId)
		{
			ImGui::Text("Collections");

			if (ImGui::BeginListBox("", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 200)))
			{
				for (int i = 0; i < collection_list.size(); i++)
				{
					ImGui::PushID(i);
					if (ImGui::Selectable(collection_list[i].c_str(), i == selection ? true : false))
					{
						selection = i;
					}
					ImGui::PopID();
				}

				ImGui::EndListBox();
			}

			static char text_buff[32];
			ImGui::InputTextWithHint("##Text", "Enter collection name", text_buff, 32);

			if (ImGui::Button("Add"))
			{
				if (text_buff[0] != 0)
				{
					collection_list.push_back(std::string(text_buff));
				}
			}

			ImGui::SameLine();

			if (ImGui::Button("Remove") && collection_list.size() > 0)
			{
				collection_list.erase(collection_list.begin() + selection);
				selection -= 1;

				if (selection < 0)
					selection = 0;
			}
		}
		ImGui::End();

		ImGui::Begin("Image Viewer");

		if (texId)
		{
			if (!image)
			{
				image = new TransformableImage(ImGui::GetCurrentWindow(), image_width, image_height);
			}

			if (!grid)
			{
				grid = new GridViewer(ImGui::GetWindowDrawList(), image_width, image_height, &density, ImGui::GetCurrentWindow());
			}

			if (ImGui::IsWindowFocused())
			{
				if (io.MouseWheel != 0)
				{
					image->Scale(io.MouseWheel * 0.07f);
					grid->Scale(io.MouseWheel * 0.07f);
				}
				else if (io.MouseDown[1])
				{
					image->Translate(io.MouseDelta.x, io.MouseDelta.y);
					grid->Translate(io.MouseDelta.x, io.MouseDelta.y);
				}
				else if (io.MouseDown[0])
				{
					grid->Select(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
				}
			}

			ImGui::GetWindowDrawList()->AddImage((ImTextureID)texId,
				ImVec2(image->GetTransformMatrix().w, image->GetTransformMatrix().x),
				ImVec2(image->GetTransformMatrix().y, image->GetTransformMatrix().z));

			grid->Render();

		}
		ImGui::End();

		// Rendering
		ImGui::Render();

		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT);

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



		glfwSwapBuffers(window);
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
