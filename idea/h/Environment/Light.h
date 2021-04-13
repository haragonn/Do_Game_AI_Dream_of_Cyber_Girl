#ifndef INCLUDE_IDEA_LIGHT_H
#define INCLUDE_IDEA_LIGHT_H

#include "../../../idea/h/Utility/ideaMath.h"

namespace idea{
	class Light{
	public:
		// ���˃x�N�g���̐ݒ�
		void SetLightDirection(Vector3D direction);

		// ���˃x�N�g���̐ݒ�
		void SetLightDirection(float directionX, float directionY, float directionZ);

		// ���˃x�N�g���̎擾
		Vector3D GetLightDirection();
	};
}

#endif	// #ifndef INCLUDE_IDEA_LIGHT_H