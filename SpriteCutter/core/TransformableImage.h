#pragma once
#include "imgui.h"
#include "imgui_internal.h"

//TODO Need to add central scaling of image viewer.

class TransformableImage
{
public:
	struct PointMatrix
	{
		float m0;
		float m1;
		float m2;

		PointMatrix operator+ (const PointMatrix& rhs);
		PointMatrix operator- (const PointMatrix& rhs);
	};

	struct Matrix 
	{
		float m0, m3, m6;
		float m1, m4, m7;
		float m2, m5, m8;

		Matrix operator* (const Matrix& rhs);
		PointMatrix operator* (const PointMatrix& rhs);
	};	

	struct Points
	{
		float w, x, y, z;
	};

	TransformableImage(ImGuiWindow* window, float image_width, float image_height);
	void Scale(float scale);
	void Translate(float x, float y);
	Points GetTransformMatrix();

private:
	float m_Xpos, m_Ypos;
	float m_ImageWidth, m_ImageHeight;
	float m_WindowWidth, m_WindowHeight; // remove and get directly from window

	ImGuiWindow* m_Window;

	Matrix m_ScaleMatrix;
	Matrix m_TranslateMatrix;
	PointMatrix m_TopPoints;
	PointMatrix m_BottomPoints;
};

