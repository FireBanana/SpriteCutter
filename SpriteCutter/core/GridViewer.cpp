#include "imgui.h"
#include "imgui_internal.h"
#include "GridViewer.h"

GridViewer::GridViewer(ImDrawList* draw_list, float image_width, float image_height, int* density, ImGuiWindow* window) :
	m_DrawList(draw_list),
	m_Density(density),
	m_ImageWidth(image_width), m_ImageHeight(image_height),
	m_Start({ window->Pos.x , window->Pos.y }),
	m_Window(window)
{
	m_ScaleMatrix = { 1.0f, 0, 0, 0, 1.0f, 0, 0, 0, 1.0f };
	m_TranslateMatrix = { 1.0f, 0, 0, 0, 1.0f, 0, 0, 0, 1.0f };

	m_TileIndexArray = new TileState[(image_width / *m_Density) * (image_height / *m_Density)];
}

GridViewer::~GridViewer()
{
	delete[] m_TileIndexArray;
}

void GridViewer::Translate(int x, int y)
{
	m_TranslateMatrix.m6 += x;
	m_TranslateMatrix.m7 += y;
}

void GridViewer::Render()
{
	TransformableImage::PointMatrix half_point = { m_ImageWidth / 2.0f, m_ImageHeight / 2.0f, 1.0f };

	for (int i = 0; i <= m_ImageWidth; i += *m_Density)
	{
		TransformableImage::PointMatrix top = { i + m_Start.x, m_Start.y };
		TransformableImage::PointMatrix bottom = { i + m_Start.x, m_Start.y + m_ImageHeight };

		TransformableImage::PointMatrix new_top = TransformPoint(top);
		TransformableImage::PointMatrix new_bottom = TransformPoint(bottom);
		
		m_DrawList->AddLine(
		ImVec2(new_top.m0, new_top.m1),
		ImVec2(new_bottom.m0, new_bottom.m1),
		ImGui::GetColorU32(ImVec4(1,0,0,1)), 4.0f);
	}

	for (int i = 0; i < m_ImageHeight; i += *m_Density)
	{
		TransformableImage::PointMatrix left = { m_Start.x, m_Start.y + i };
		TransformableImage::PointMatrix right = { m_Start.x + m_ImageWidth, m_Start.y + i };

		TransformableImage::PointMatrix new_left = TransformPoint(left);
		TransformableImage::PointMatrix new_right = TransformPoint(right);

		m_DrawList->AddLine(
			ImVec2(new_left.m0, new_left.m1),
			ImVec2(new_right.m0, new_right.m1),
			ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), 4.0f);
	}

	int width = m_ImageWidth / *m_Density;
	int xIndex = 0, yIndex = 0;

	for (int i = 0; i < m_ImageWidth; i += *m_Density)
	{
		for (int j = 0; j < m_ImageHeight; j += *m_Density)
		{
			if (m_TileIndexArray[yIndex * width + xIndex].selected)
			{
				TransformableImage::PointMatrix point = TransformPoint(TransformableImage::PointMatrix{ (float)i + m_Start.x, (float)j + m_Start.y, 1 });

				m_DrawList->AddCircle(ImVec2(point.m0 + 3, point.m1 + 3), 3, IM_COL32_WHITE);
			}
			yIndex++;
		}
		xIndex++;
		yIndex = 0;
	}
}

void GridViewer::Scale(float scale)
{
	m_ScaleMatrix.m0 += scale;
	m_ScaleMatrix.m4 += scale;
}

void GridViewer::Select(int x, int y)
{
	int xIndex = 0, yIndex = 0;
	int width = m_ImageWidth / *m_Density;

	for (int i = 0; i <= m_ImageWidth; i += *m_Density)
	{
		for (int j = 0; j <= m_ImageHeight; j += *m_Density)
		{
			TransformableImage::PointMatrix point = TransformPoint(TransformableImage::PointMatrix{ (float)i + m_Start.x, (float)j + m_Start.y, 1 });

			if (x > point.m0 && x < point.m0 + *m_Density &&
				y > point.m1 && y < point.m1 + *m_Density)
			{
				m_TileIndexArray[yIndex * width + xIndex].selected = true;
				return;
			}

			yIndex++;
		}
		xIndex++;
		yIndex = 0;
	}
}

void GridViewer::ScaleTiles()
{
	delete[] m_TileIndexArray;
	m_TileIndexArray = new TileState[(m_ImageWidth / *m_Density) * (m_ImageHeight / *m_Density)];
}

inline TransformableImage::PointMatrix GridViewer::TransformPoint(const TransformableImage::PointMatrix& point)
{
	TransformableImage::PointMatrix half_point = { m_ImageWidth / 2.0f, m_ImageHeight / 2.0f, 1.0f };

	return m_TranslateMatrix * 
		TransformableImage::PointMatrix(half_point + m_ScaleMatrix * (TransformableImage::PointMatrix({ point.m0 , point.m1, 1.0f }) - half_point));
}
