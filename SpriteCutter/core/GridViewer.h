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
		bool selected;
		int layer;
		std::string layer_name;

		TileState() : 
			selected(false), layer(0), layer_name("default")
		{}

		TileState(bool s, int l, std::string ss) :
			selected(s), layer(l), layer_name(ss) {}
	};

	GridViewer(ImDrawList* draw_list, float image_width, float image_height, int* density, ImGuiWindow* window);
	~GridViewer();
	
	void Translate(int x, int y);
	void Render();
	void RenderGUI();
	void Scale(float scale);
	void Select(int x, int y, int& layerIndex, std::string& lName);
	void Remove(const std::string& lName);
	void ScaleTiles();
	void SetTileState(std::vector<TileState> state_array);

	inline TileState* GetStateArray() { return m_TileIndexArray; }
	inline int GetStateArraySize() { return m_TileIndexArraySize; }

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

	bool m_IsDraw; //Draw State

	ImColor* m_LayerColors;

	inline TransformableImage::PointMatrix TransformPoint(const TransformableImage::PointMatrix& pixel_division);
};

