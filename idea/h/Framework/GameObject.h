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

		// ������
		virtual void Init() = 0;

		// �I������
		virtual void UnInit()
		{}

		// �X�V
		virtual void Update()
		{}

		// �`��
		virtual void Draw()
		{}

		// �j��
		inline void SetDestroy()
		{
			bDestroy_ = true;
		}

		// ���W�̐ݒ�
		inline void SetPosition(const Vector3D& position)
		{
			position_ = position;
		}

		// ���W�̐ݒ�
		inline void SetPosition(float positionX, float positionY, float positionZ)
		{
			position_ = Vector3D(positionX, positionY, positionZ);
		}

		// ���W�̈ړ�
		inline void MovePosition(const Vector3D& axis)
		{
			position_ += axis;
		}

		// ���W�̈ړ�
		inline void MovePosition(float axisX, float axisY, float axisZ)
		{
			position_.x += axisX;
			position_.y += axisY;
			position_.z += axisZ;
		}

		// ���W�̎擾
		inline const Vector3D& GetPosition()const
		{
			return position_;
		}

		// ��]�p�̐ݒ�
		inline void SetRotation(const Vector3D& rotation)
		{
			rotation_ = rotation;
		}

		// ��]�p�̐ݒ�
		inline void SetRotation(float pitch, float yaw, float roll)
		{
			rotation_.x = pitch;
			rotation_.y = yaw;
			rotation_.z = roll;
		}

		// ��]
		inline void MoveRotation(const Vector3D& rotation)
		{
			rotation_ += rotation;
		}

		// ��]
		inline void MoveRotation(float pitch, float yaw, float roll)
		{
			rotation_.x += pitch;
			rotation_.y += yaw;
			rotation_.z += roll;
		}

		// ��]�p�̎擾
		inline const Vector3D& GetRotation()const
		{
			return rotation_;
		}

		// �g�k�{���̐ݒ�
		inline void SetScale(const Vector3D& scale)
		{
			scale_ = scale;
		}

		// �g�k�{���̐ݒ�
		inline void SetScale(float scaleX, float scaleY, float scaleZ)
		{
			scale_.x = scaleX;
			scale_.y = scaleY;
			scale_.z = scaleZ;
		}

		// �g�k�{���̎擾
		inline const Vector3D& GetScale()const
		{
			return scale_;
		}

		// �O���x�N�g���̎擾
		inline Vector3D GetForward()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._31, rot._32, rot._33);
		}

		// �E�x�N�g���̎擾
		inline Vector3D GetRight()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._11, rot._12, rot._13);
		}

		// ��x�N�g���̎擾
		inline Vector3D GetUp()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._21, rot._22, rot._23);
		}

	protected:
		// ���W
		Vector3D position_;

		// ��]�p
		Vector3D rotation_;

		// �g�k
		Vector3D scale_ = { 1.0f, 1.0f, 1.0f };

		// ���݂̃V�[���̃|�C���^
		Scene* pScene_ = nullptr;

		// �j���t���O
		bool bDestroy_ = false;

	private:
		friend class Scene;

		// �j���̎��s
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