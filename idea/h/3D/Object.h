/*==============================================================================
	[Object.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_OBJECT_H
#define INCLUDE_IDEA_OBJECT_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Actor3D.h"
#include "Collision3D.h"
#include "../Environment/Camera.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FObject
	// �N���X�T�v�FActor3D���p�������N���X,�J�����ɉf����̂̊��N���X
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

		// ������ێ�����J�����̃��X�g
		std::vector<Camera*> vecCameraPtr_;

		SphereCollider sc;

		// �`��
		virtual void Draw(Camera* pCamera) = 0;
	};
}
#endif	// #ifndef INCLUDE_IDEA_OBJECT_H