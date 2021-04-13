/*==============================================================================
	[Actor3D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ACTOR3D_H
#define INCLUDE_IDEA_ACTOR3D_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"
#include "../3D/3DStruct.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FActor3D
	// �N���X�T�v�F3D��ł̍��W,�傫��,��],�F�����N���X
	//------------------------------------------------------------------------------
	class Actor3D{
	public:
		Actor3D();
		virtual ~Actor3D();

		// ������
		virtual void Init(const Vector3D& position);

		// ������
		virtual void Init(float positionX, float positionY, float positionZ);

		// �I��
		virtual void UnInit();

		// ���W�̐ݒ�
		void SetPosition(const Vector3D& position);

		// ���W�̐ݒ�
		void SetPosition(float positionX, float positionY, float positionZ);

		// �O�t���[���̍��W���ƍ��W��ݒ�
		void SetBasePosition(const Vector3D& position);

		// �O�t���[���̍��W���ƍ��W��ݒ�
		void SetBasePosition(float positionX, float positionY, float positionZ);

		// ���W�̈ړ�
		void MovePosition(const Vector3D& axis);

		// ���W�̈ړ�
		void MovePosition(float axisX, float axisY, float axisZ);

		// ���W�̎擾
		const Vector3D& GetPosition()const{ return position_; }

		// ��]�p�̐ݒ�
		void SetRotation(const Vector3D& rot);

		// ��]�p�̐ݒ�
		void SetRotation(float pitch, float yaw, float roll);

		// ��]
		void MoveRotation(const Vector3D& rot);

		// ��]
		void MoveRotation(float pitch, float yaw, float roll);

		// ��]�p�̎擾
		const Vector3D& GetLocalRotation()const{ return rotation_; }

		// ��]�s��̎擾
		const Matrix4x4& GetRotationMatrix()const{ return rotationMatrix_; }

		// �g�k�{���̐ݒ�
		void SetScale(const Vector3D& scale);

		// �g�k�{���̐ݒ�
		void SetScale(float scaleX, float scaleY, float scaleZ);

		// �g�k�{���̎擾
		const Vector3D& GetScale()const{ return scale_; }

		// ��ދ��̔��a�̔{���̎擾
		const float GetLengthRate()const{ return lengthRate_; }

		// �F�̐ݒ�
		virtual void SetColor(const Color& color);

		// �F�̐ݒ�
		virtual void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// �F�̐ݒ�
		void SetDiffuse(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// �F�̐ݒ�
		void SetDiffuse(const Color& color);

		// �F�̐ݒ�
		void SetAmbient(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// �F�̐ݒ�
		void SetAmbient(const Color& color);

		// �F�̐ݒ�
		void SetSpecular(const Color& color);

		// �F�̐ݒ�
		void SetSpecular(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// ���x�̐ݒ�
		void SetShininess(float shininess);

		// �F�̎擾
		const Color& GetColor()const{ return color_; }

		// �q�̃Z�b�g
		void SetChild(Actor3D& child);

	protected:
		// ���W
		Vector3D position_;

		// �O�t���[���̍��W
		Vector3D prePosition_;

		// ��]�p
		Vector3D rotation_;

		// �g�k�{��
		Vector3D scale_;

		// ��ދ��̔��a�̔{��
		float lengthRate_ = 0.0f;

		// �F
		Color color_;

		// �}�e���A��
		Material material_;

		// ��]�s��
		Matrix4x4 rotationMatrix_;

		// ���[���h�}�g���N�X
		Matrix4x4 world_;

		// �q
		std::vector<Actor3D*> children_;

		// ���[���h�}�g���N�X�̍X�V
		virtual inline void UpdateWorldMatrix();

		// ���[���h�}�g���N�X�̍X�V
		virtual inline void UpdateWorldMatrix(Matrix4x4& parentMatrix);

	private:
		friend class Actor3DManager;

		// �o�^�t���O
		bool bRegistered_ = false;

		// ���W�̕ێ�
		void UpdatePrePosition();
	};
}

#endif	// #ifndef INCLUDE_IDEA_ACTOR3D_H