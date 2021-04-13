#ifndef INCLUDE_IDEA_GAMEOBJECT_H
#define INCLUDE_IDEA_GAMEOBJECT_H

#include "../Utility/ideaMath.h"
#include "../Framework/GameManager.h"

namespace idea{
	class GameObject{
	public:
		GameObject() :
			bDestroy_(false)
		{
			GameManager gameManager;

			pScene_ = gameManager.GetScenePtr();
		}

		virtual ~GameObject() = default;

		// ‰Šú‰»
		virtual void Init() = 0;

		// I—¹ˆ—
		virtual void UnInit()
		{}

		// XV
		virtual void Update()
		{}

		// •`‰æ
		virtual void Draw()
		{}

		// ”jŠü
		inline void SetDestroy()
		{
			bDestroy_ = true;
		}

		// À•W‚Ìİ’è
		inline void SetPosition(const Vector3D& position)
		{
			position_ = position;
		}

		// À•W‚Ìİ’è
		inline void SetPosition(float positionX, float positionY, float positionZ)
		{
			position_ = Vector3D(positionX, positionY, positionZ);
		}

		// À•W‚ÌˆÚ“®
		inline void MovePosition(const Vector3D& axis)
		{
			position_ += axis;
		}

		// À•W‚ÌˆÚ“®
		inline void MovePosition(float axisX, float axisY, float axisZ)
		{
			position_.x += axisX;
			position_.y += axisY;
			position_.z += axisZ;
		}

		// À•W‚Ìæ“¾
		inline const Vector3D& GetPosition()const
		{
			return position_;
		}

		// ‰ñ“]Šp‚Ìİ’è
		inline void SetRotation(const Vector3D& rotation)
		{
			rotation_ = rotation;
		}

		// ‰ñ“]Šp‚Ìİ’è
		inline void SetRotation(float pitch, float yaw, float roll)
		{
			rotation_.x = pitch;
			rotation_.y = yaw;
			rotation_.z = roll;
		}

		// ‰ñ“]
		inline void MoveRotation(const Vector3D& rotation)
		{
			rotation_ += rotation;
		}

		// ‰ñ“]
		inline void MoveRotation(float pitch, float yaw, float roll)
		{
			rotation_.x += pitch;
			rotation_.y += yaw;
			rotation_.z += roll;
		}

		// ‰ñ“]Šp‚Ìæ“¾
		inline const Vector3D& GetRotation()const
		{
			return rotation_;
		}

		// Šgk”{—¦‚Ìİ’è
		inline void SetScale(const Vector3D& scale)
		{
			scale_ = scale;
		}

		// Šgk”{—¦‚Ìİ’è
		inline void SetScale(float scaleX, float scaleY, float scaleZ)
		{
			scale_.x = scaleX;
			scale_.y = scaleY;
			scale_.z = scaleZ;
		}

		// Šgk”{—¦‚Ìæ“¾
		inline const Vector3D& GetScale()const
		{
			return scale_;
		}

		// ‘O•ûƒxƒNƒgƒ‹‚Ìæ“¾
		inline Vector3D GetForward()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._31, rot._32, rot._33);
		}

		// ‰EƒxƒNƒgƒ‹‚Ìæ“¾
		inline Vector3D GetRight()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._11, rot._12, rot._13);
		}

		// ãƒxƒNƒgƒ‹‚Ìæ“¾
		inline Vector3D GetUp()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._21, rot._22, rot._23);
		}

	protected:
		// À•W
		Vector3D position_;

		// ‰ñ“]Šp
		Vector3D rotation_;

		// Šgk
		Vector3D scale_ = { 1.0f, 1.0f, 1.0f };

		// Œ»İ‚ÌƒV[ƒ“‚Ìƒ|ƒCƒ“ƒ^
		Scene* pScene_ = nullptr;

		// ”jŠüƒtƒ‰ƒO
		bool bDestroy_ = false;

	private:
		friend class Scene;

		// ”jŠü‚ÌÀs
		bool Destroy()
		{
			if(!bDestroy_){
				return false;
			}

			UnInit();

			delete this;

			return true;
		}
	};
}

#endif	// #ifndef INCLUDE_IDEA_GAMEOBJECT_H