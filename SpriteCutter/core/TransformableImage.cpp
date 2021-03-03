#include "TransformableImage.h"
#include <cstring>

TransformableImage::TransformableImage(ImGuiWindow* window, float image_width, float image_height) :
	m_Xpos(window->Pos.x), m_Ypos(window->Pos.y),
	m_ImageWidth(image_width), m_ImageHeight(image_height),
	m_WindowWidth(window->Size.x), m_WindowHeight(window->Size.y),
	m_Window(window)
{
	m_ScaleMatrix = { 1.0f, 0, 0, 0, 1.0f, 0, 0, 0, 1.0f };
	m_TranslateMatrix = {1.0f, 0, 0, 0, 1.0f, 0, 0, 0, 1.0f};

	m_TopPoints.m0 = m_Xpos;
	m_TopPoints.m1 = m_Ypos;
	m_TopPoints.m2 = 1.0f;

	m_BottomPoints.m0 = m_Xpos + m_ImageWidth;
	m_BottomPoints.m1 = m_Ypos + m_ImageHeight;
	m_BottomPoints.m2 = 1.0f;
}

void TransformableImage::Scale(float scale)
{
	m_ScaleMatrix.m0 += scale;
	m_ScaleMatrix.m4 += scale;
}

void TransformableImage::Translate(float x, float y)
{
	m_TranslateMatrix.m6 += x;
	m_TranslateMatrix.m7 += y;
}

TransformableImage::Points TransformableImage::GetTransformMatrix()
{
	Points p;
	PointMatrix top, bottom;

	top = m_TranslateMatrix * (PointMatrix({ m_ImageWidth / 2.0f, m_ImageHeight / 2.0f, 1.0f }) + (m_ScaleMatrix * (PointMatrix({ m_TopPoints.m0 - m_ImageWidth / 2.0f, m_TopPoints.m1 - m_ImageHeight / 2.0f, 1.0f }))));
	bottom = m_TranslateMatrix * (PointMatrix({ m_ImageWidth / 2.0f, m_ImageHeight / 2.0f, 1.0f }) + (m_ScaleMatrix * (PointMatrix({ m_BottomPoints.m0 - m_ImageWidth / 2.0f, m_BottomPoints.m1 - m_ImageHeight / 2.0f, 1.0f }))));

	p.w = top.m0;
	p.x = top.m1;
	p.y = bottom.m0;
	p.z = bottom.m1;

	return p;
}

TransformableImage::Matrix TransformableImage::Matrix::operator*(const Matrix& rhs)
{
	Matrix m;

	m.m0 = this->m0 * rhs.m0 + this->m3 * rhs.m1 + this->m6 * rhs.m2;
	m.m1 = this->m1 * rhs.m0 + this->m4 * rhs.m1 + this->m7 * rhs.m2;
	m.m2 = this->m2 * rhs.m0 + this->m5 * rhs.m1 + this->m8 * rhs.m2;

	m.m3 = this->m0 * rhs.m3 + this->m3 * rhs.m4 + this->m6 * rhs.m5;
	m.m4 = this->m1 * rhs.m3 + this->m4 * rhs.m4 + this->m7 * rhs.m5;
	m.m5 = this->m2 * rhs.m3 + this->m5 * rhs.m4 + this->m8 * rhs.m5;

	m.m6 = this->m0 * rhs.m6 + this->m3 * rhs.m7 + this->m6 * rhs.m8;
	m.m7 = this->m1 * rhs.m6 + this->m4 * rhs.m7 + this->m7 * rhs.m8;
	m.m8 = this->m2 * rhs.m6 + this->m5 * rhs.m7 + this->m8 * rhs.m8;

	return m;
}

TransformableImage::PointMatrix TransformableImage::Matrix::operator*(const PointMatrix& rhs)
{
	PointMatrix m;

	m.m0 = this->m0 * rhs.m0 + this->m3 * rhs.m1 + this->m6 * rhs.m2;
	m.m1 = this->m1 * rhs.m0 + this->m4 * rhs.m1 + this->m7 * rhs.m2;
	m.m2 = this->m2 * rhs.m0 + this->m5 * rhs.m1 + this->m8 * rhs.m2;

	return m;
}

TransformableImage::PointMatrix TransformableImage::PointMatrix::operator+(const PointMatrix& rhs)
{
	PointMatrix m;

	m.m0 = this->m0 + rhs.m0;
	m.m1 = this->m1 + rhs.m1;
	m.m2 = 1.0f;

	return m;
}

TransformableImage::PointMatrix TransformableImage::PointMatrix::operator-(const PointMatrix& rhs)
{
	PointMatrix m;

	m.m0 = this->m0 - rhs.m0;
	m.m1 = this->m1 - rhs.m1;
	m.m2 = 1.0f;

	return m;
}
