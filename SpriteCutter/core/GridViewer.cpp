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

	m_TileIndexArraySize = (image_width / *m_Density) * (image_height / *m_Density);
	m_TileIndexArray = new TileState[m_TileIndexArraySize];
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
	int imageHeight = m_ImageHeight / *m_Density;
	int imageWidth = m_ImageWidth / *m_Density;


	for (int i = 0; i < m_ImageWidth; i += *m_Density)
	{
		TransformableImage::PointMatrix top = { i + m_Start.x, m_Start.y };
		TransformableImage::PointMatrix bottom = { i + m_Start.x, m_Start.y + (imageHeight * *m_Density) };

		TransformableImage::PointMatrix new_top = TransformPoint(top);
		TransformableImage::PointMatrix new_bottom = TransformPoint(bottom);

		m_DrawList->AddLine(
			ImVec2(new_top.m0, new_top.m1),
			ImVec2(new_bottom.m0, new_bottom.m1),
			ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), 4.0f);
	}

	for (int i = 0; i < m_ImageHeight; i += *m_Density)
	{
		TransformableImage::PointMatrix left = { m_Start.x, m_Start.y + i };
		TransformableImage::PointMatrix right = { m_Start.x + (imageWidth * *m_Density), m_Start.y + i };

		TransformableImage::PointMatrix new_left = TransformPoint(left);
		TransformableImage::PointMatrix new_right = TransformPoint(right);

		m_DrawList->AddLine(
			ImVec2(new_left.m0, new_left.m1),
			ImVec2(new_right.m0, new_right.m1),
			ImGui::GetColorU32(ImVec4(1, 0, 0, 1)), 4.0f);
	}

	int width = m_ImageWidth / *m_Density;

	for (int index = 0; index < m_TileIndexArraySize; index++)
	{
		int xIndex = index % width;
		int yIndex = index / width;

		if (m_TileIndexArray[yIndex * width + xIndex].selected)
		{
			TransformableImage::PointMatrix point = TransformPoint(TransformableImage::PointMatrix{ ((float)xIndex * *m_Density) + m_Start.x, ((float)yIndex * *m_Density) + m_Start.y, 1 });
			TransformableImage::PointMatrix point2 = TransformPoint(TransformableImage::PointMatrix{ ((float)xIndex * *m_Density) + m_Start.x + *m_Density, ((float)yIndex * *m_Density) + m_Start.y + *m_Density, 1 });

			m_DrawList->AddRectFilled(ImVec2(point.m0, point.m1), ImVec2(point2.m0, point2.m1), IM_COL32_WHITE);
		}
	}
}

void GridViewer::Scale(float scale)
{
	m_ScaleMatrix.m0 += scale;
	m_ScaleMatrix.m4 += scale;
}

void GridViewer::Select(int x, int y)
{
	int width = m_ImageWidth / *m_Density;
	int height = m_ImageHeight / *m_Density;

	for (int index = 0; index < m_TileIndexArraySize; index++)
	{
		int xIndex = index % width;
		int yIndex = index / width;

		TransformableImage::PointMatrix point = TransformPoint(TransformableImage::PointMatrix{ ((float)xIndex * *m_Density) + m_Start.x, ((float)yIndex * *m_Density) + m_Start.y, 1 });
		TransformableImage::PointMatrix point2 = TransformPoint(TransformableImage::PointMatrix{ ((float)xIndex * *m_Density) + m_Start.x + *m_Density, ((float)yIndex * *m_Density) + m_Start.y + *m_Density, 1 });

		if (x > point.m0 && x < point2.m0 &&
			y > point.m1 && y < point2.m1)
		{
			m_TileIndexArray[yIndex * width + xIndex].selected = true;
			return;
		}
	}

	//

	/*int xIndex = 0, yIndex = 0;

	for (int i = 0; i <= m_ImageWidth; i += *m_Density)
	{
		for (int j = 0; j <= m_ImageHeight; j += *m_Density)
		{
			TransformableImage::PointMatrix point = TransformPoint(TransformableImage::PointMatrix{ (float)i + m_Start.x, (float)j + m_Start.y, 1 });
			TransformableImage::PointMatrix point2 = TransformPoint(TransformableImage::PointMatrix{ (float)i + m_Start.x + *m_Density, (float)j + m_Start.y + *m_Density, 1 });

			if (yIndex != width - 1 && ) {
				if (x > point.m0 && x < point2.m0 &&
					y > point.m1 && y < point2.m1)
				{
					m_TileIndexArray[yIndex * width + xIndex].selected = true;
					return;
				}
			}

			yIndex++;
		}
		xIndex++;
		yIndex = 0;
	}*/
}

void GridViewer::ScaleTiles()
{
	delete[] m_TileIndexArray;
	m_TileIndexArraySize = (m_ImageWidth / *m_Density) * (m_ImageHeight / *m_Density);
	m_TileIndexArray = new TileState[m_TileIndexArraySize];
}

inline TransformableImage::PointMatrix GridViewer::TransformPoint(const TransformableImage::PointMatrix& point)
{
	TransformableImage::PointMatrix half_point = { m_ImageWidth / 2.0f, m_ImageHeight / 2.0f, 1.0f };

	return m_TranslateMatrix *
		TransformableImage::PointMatrix(half_point + m_ScaleMatrix * (TransformableImage::PointMatrix({ point.m0 , point.m1, 1.0f }) - half_point));
}
