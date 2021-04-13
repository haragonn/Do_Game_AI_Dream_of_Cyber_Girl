#ifndef INCLUDE_IDEA_LIGHT_H
#define INCLUDE_IDEA_LIGHT_H

#include "../../../idea/h/Utility/ideaMath.h"

namespace idea{
	class Light{
	public:
		// 入射ベクトルの設定
		void SetLightDirection(Vector3D direction);

		// 入射ベクトルの設定
		void SetLightDirection(float directionX, float directionY, float directionZ);

		// 入射ベクトルの取得
		Vector3D GetLightDirection();
	};
}

#endif	// #ifndef INCLUDE_IDEA_LIGHT_H