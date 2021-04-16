#pragma once
#include "TransformableImage.h"
#include "GridViewer.h"
#include "../stb_image.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer
{
private:
	int m_WindowWidth, m_WindowHeight;
	GLFWwindow* m_Window;
	ImGuiIO* m_IO;

	TransformableImage* m_DisplayImage;
	unsigned int m_DisplayImageTextureId;
	int m_ImageWidth, m_ImageHeight;

	GridViewer* m_GridViewer;
	const char* m_ImagePath;

	std::vector<GridViewer::TileState> m_LoadedTileState;
	std::vector<std::string> m_LayerList;
	int m_Selection;

	int m_Divisions;
	bool m_ThrowError;
	const char* m_ErrorMessage;

	bool LoadTextureFromFile(const char* filename, unsigned int* out_texture, int* out_width, int* out_height);
	inline void Throw(const char* mssg) { m_ThrowError = true; m_ErrorMessage = mssg; }

public:
	Renderer(int width, int height);
	~Renderer();

	void Initialize();
	void Update();

	inline GLFWwindow* GetWindow() const { return m_Window; }
};

