#include "../../h/Mesh/LineMesh.h"
#define WIN32_LEAN_AND_MEAN
#include <DirectXMath.h>

using namespace DirectX;
using namespace idea;

bool LineMesh::Create(float thickness)
{
	Actor3D::Init(0.0f, 0.0f, 0.0f);
	Actor3D::SetScale(0.0f, 0.0f, 0.0f);


	thickness_ = thickness;

	return true;
}

void LineMesh::SetPoint(Vector3D startPoint, Vector3D endPoint)
{
	startPoint_ = startPoint;
	endPoint_ = endPoint;
	Vector3D v = endPoint_ - startPoint_;

	Actor3D::SetPosition((v * 0.5f) + startPoint_);

	Actor3D::SetRotation(v.PitchYaw().ReplaceZ(DegreeToRadian(45.0f)));

	Actor3D::SetScale(thickness_, thickness_, v.Length());

	UpdateWorldMatrix();
}