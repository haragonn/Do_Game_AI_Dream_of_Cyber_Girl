#include "../../h/Environment/Light.h"
#include "../../h/3D/ObjectManager.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;
using namespace idea;

void Light::SetLightDirection(Vector3D direction)
{
	SetLightDirection(direction.x, direction.y, direction.z);
}

void Light::SetLightDirection(float directionX, float directionY, float directionZ)
{
	ObjectManager::Instance().SetLight(directionX, directionY, directionZ);
}

Vector3D Light::GetLightDirection()
{
	ObjectManager om = ObjectManager::Instance();
	return Vector3D(om.GetLight().x, om.GetLight().y, om.GetLight().z);
}
