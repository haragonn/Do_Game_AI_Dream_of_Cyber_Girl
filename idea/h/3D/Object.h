/*==============================================================================
	[Object.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_OBJECT_H
#define INCLUDE_IDEA_OBJECT_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Actor3D.h"
#include "Collision3D.h"
#include "../Environment/Camera.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Object
	// クラス概要：Actor3Dを継承したクラス,カメラに映るものの基底クラス
	//------------------------------------------------------------------------------
	class Object : public Actor3D{
	public:
		virtual ~Object()
		{
			if(vecCameraPtr_.size() > 0){
				for(auto it = std::begin(vecCameraPtr_), itEnd = std::end(vecCameraPtr_); it != itEnd; ++it){
					if(*it){
						(*it)->RemoveObject(*this);
					}
				}
			}
		}

		virtual BaseCollider* GetColliderPtr()
		{
			sc.SetSphere(position_, 0.0f, prePosition_);

			return &sc;
		}

	protected:
		friend class Camera;
		friend class ShadowCamera;

		// 自分を保持するカメラのリスト
		std::vector<Camera*> vecCameraPtr_;

		SphereCollider sc;

		// 描画
		virtual void Draw(Camera* pCamera) = 0;
	};
}
#endif	// #ifndef INCLUDE_IDEA_OBJECT_H