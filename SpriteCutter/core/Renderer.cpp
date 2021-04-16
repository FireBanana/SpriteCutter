#include "Renderer.h"
#include <iostream>
#include <Windows.h>
#include <commdlg.h>
#include "imgui.h"
#include "Util.h"
#include "FileMaker.h"
#include "../imgui_impl_glfw.h"
#include "../imgui_impl_opengl3.h"

Renderer::Renderer(int width, int height)
	: m_WindowWidth(width), m_WindowHeight(height), m_Divisions(16)
{
}

Renderer::~Renderer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_Window);
	glfwTerminate();
}

void Renderer::Initialize()
{
	if (!glfwInit())
		throw std::exception();

	m_Window = glfwCreateWindow(m_WindowWidth, m_WindowHeight, "Sprite Cutter", NULL, NULL);
	if (m_Window == NULL)
		throw std::exception();

	glfwMakeContextCurrent(m_Window);
	glfwSwapInterval(1);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	ImGui::CreateContext();

	m_IO = &(ImGui::GetIO());
	m_IO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGuiStyle* style = &ImGui::GetStyle();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init();

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
}

void Renderer::Update()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::DockSpaceOverViewport();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New File"))
			{
				CREATE_FILE_STRUCT();

				if (GetOpenFileName(&ofn) == TRUE)
				{
					CleanPointer(m_DisplayImage);
					CleanPointer(m_GridViewer);

					const size_t path_size = strlen(ofn.lpstrFile);
					char* buffer = new char[path_size + 1];
					buffer[path_size] = '\0';
					strcpy_s(buffer, path_size + 1, ofn.lpstrFile);
					m_ImagePath = buffer;

					LoadTextureFromFile(ofn.lpstrFile, &m_DisplayImageTextureId, &m_ImageWidth, &m_ImageHeight);

					m_LoadedTileState.clear();
				}
			}

			if (ImGui::MenuItem("Import File"))
			{
				SCFile file = LoadFile();

				if (!file.is_corrupt)
				{
					CleanPointer(m_DisplayImage);
					CleanPointer(m_GridViewer);

					delete[] m_ImagePath;
					m_ImagePath = file.path.c_str();
					LoadTextureFromFile(m_ImagePath, &m_DisplayImageTextureId, &m_ImageWidth, &m_ImageHeight);

					m_LoadedTileState = file.state_array;
					m_Divisions = file.divisions;
					m_LayerList = file.collection_list;
				}
				else
					Throw("File is corrupt");
			}

			if (m_LayerList.size() > 0 && ImGui::MenuItem("Export File"))
			{
				MakeFile(m_GridViewer->GetStateArray(), m_GridViewer->GetStateArraySize(), m_ImagePath, m_Divisions, m_LayerList);
			}

			ImGui::EndMenu();
		}

		if (m_DisplayImageTextureId && m_LayerList.size() > 0)
		{
			ImGui::SetCursorPos(ImVec2(ImGui::GetWindowWidth() - 175, 0));
			ImGui::PushItemWidth(50);
			if (ImGui::DragInt("Division Size", &m_Divisions, 0.05f, 16, 1000))
			{
				m_GridViewer->ScaleTiles();
			}
		}


		ImGui::EndMainMenuBar();
	}
	ImGui::PopStyleVar();

	ImGui::Begin("Sprite Tool");
	if (m_DisplayImageTextureId)
	{
		ImGui::Text("Collections");

		if (ImGui::BeginListBox("", ImVec2(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - 200)))
		{
			for (int i = 0; i < m_LayerList.size(); i++)
			{
				ImGui::PushID(i);
				if (ImGui::Selectable(m_LayerList[i].c_str(), i == m_Selection ? true : false))
				{
					m_Selection = i;
				}
				ImGui::PopID();
			}

			ImGui::EndListBox();
		}

		static char text_buff[32];
		ImGui::InputTextWithHint("##Text", "Enter collection name", text_buff, 32);

		if (ImGui::Button("Add"))
		{
			if (text_buff[0] != 0 && m_LayerList.size() < 8)
			{
				m_LayerList.push_back(std::string(text_buff));
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Remove") && m_LayerList.size() > 0)
		{
			m_GridViewer->Remove(m_LayerList[m_Selection]);

			m_LayerList.erase(m_LayerList.begin() + m_Selection);
			m_Selection -= 1;

			if (m_Selection < 0)
				m_Selection = 0;
		}
	}
	ImGui::End();

	ImGui::Begin("Image Viewer");
	if (m_DisplayImageTextureId && m_LayerList.size() > 0)
	{
		if (!m_DisplayImage)
		{
			m_DisplayImage = new TransformableImage(ImGui::GetCurrentWindow(), m_ImageWidth, m_ImageHeight);
		}

		if (!m_GridViewer)
		{
			m_GridViewer = new GridViewer(ImGui::GetWindowDrawList(), m_ImageWidth, m_ImageHeight, &m_Divisions, ImGui::GetCurrentWindow());

			if (m_LoadedTileState.size() > 0)
				m_GridViewer->SetTileState(m_LoadedTileState);
		}

		if (ImGui::IsWindowHovered())
		{
			if (m_IO->MouseWheel != 0)
			{
				m_DisplayImage->Scale(m_IO->MouseWheel * 0.07f);
				m_GridViewer->Scale(m_IO->MouseWheel * 0.07f);
			}
			else if (m_IO->MouseDown[1])
			{
				m_DisplayImage->Translate(m_IO->MouseDelta.x, m_IO->MouseDelta.y);
				m_GridViewer->Translate(m_IO->MouseDelta.x, m_IO->MouseDelta.y);
			}
			else if (m_IO->MouseDown[0])
			{
				m_GridViewer->Select(ImGui::GetMousePos().x, ImGui::GetMousePos().y, m_Selection, m_LayerList[m_Selection]);
			}
		}

		ImGui::GetWindowDrawList()->AddImage((ImTextureID)m_DisplayImageTextureId,
			ImVec2(m_DisplayImage->GetTransformMatrix().w, m_DisplayImage->GetTransformMatrix().x),
			ImVec2(m_DisplayImage->GetTransformMatrix().y, m_DisplayImage->GetTransformMatrix().z));

		m_GridViewer->Render();
		m_GridViewer->RenderGUI();

	}
	ImGui::End();

	if (m_ThrowError)
	{
		m_ThrowError = false;
		ImGui::OpenPopup("corrupt");
	}

	if (ImGui::BeginPopupModal("corrupt", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text(m_ErrorMessage);
		if (ImGui::Button("Close")) ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	// Rendering
	ImGui::Render();

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_Window);

	glfwPollEvents();
}

bool Renderer::LoadTextureFromFile(const char* filename, unsigned int* out_texture, int* out_width, int* out_height)
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
