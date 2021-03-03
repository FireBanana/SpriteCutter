#pragma once
#include "TransformableImage.h"
#include <vector>
#include <iostream>

class GridViewer
{
public:
	struct Points
	{
		float x, y;
	};

	struct TileState
	{
		bool selected = false;
	};

	GridViewer(ImDrawList* draw_list, float image_width, float image_height, int* density, ImGuiWindow* window);
	~GridViewer();
	
	void Translate(int x, int y);
	void Render();
	void Scale(float scale);
	void Select(int x, int y);
	void ScaleTiles();

private:
	int* m_Density;

	ImDrawList* m_DrawList;
	ImGuiWindow* m_Window;

	float m_ImageWidth, m_ImageHeight;
	TransformableImage::Matrix m_TranslateMatrix;
	TransformableImage::Matrix m_ScaleMatrix;
	Points m_Start;

	int m_TileIndexArraySize;
	TileState* m_TileIndexArray;

	inline TransformableImage::PointMatrix TransformPoint(const TransformableImage::PointMatrix& pixel_division);
};

