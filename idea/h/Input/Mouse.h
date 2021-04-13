#ifndef INCLUDE_IDEA_MOUSE_H
#define INCLUDE_IDEA_MOUSE_H

#include "../Environment/Camera.h"
#include "../3D/Object.h"
#include "../Utility/ideaMath.h"

namespace idea{
	class Mouse{
	public:
		// É{É^ÉìÇÃç≈ëÂêî
		static const int BUTTON_MAX = 4;

		Vector2D GetPosition();

		Vector2D GetDifference();

		int GetButton(unsigned int buttonID);

		float GetWheel();

		Segment3D SegmentToWorld(const Camera& camera);

		Vector3D PositionOnSegmentFromOBB(const Camera& camera, BoxCollider bc);

		bool CheckHitRayToSphere(const Camera& camera, const Object& object);
	};
}

#endif // #ifndef INCLUDE_IDEA_MOUSE_H