#ifndef INCLUDE_IDEA_CAMERA_H
#define INCLUDE_IDEA_CAMERA_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../../h/3D/Collision3D.h"
#include "../../h/Utility/ideaMath.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// �O���錾
	//------------------------------------------------------------------------------
	class Object;

	//------------------------------------------------------------------------------
	// �N���X���@�FCamera
	// �N���X�T�v�F3D�I�u�W�F�N�g��`�悷��ׂ̃J�����̃N���X
	//------------------------------------------------------------------------------
	class Camera{
	public:
		// �r���[�|�[�g
		struct ViewPort{
			float topLeftX;
			float topLeftY;
			float width;
			float height;
			float minDepth;
			float maxDepth;
		};

		Camera();
		virtual ~Camera();

		// ������
		virtual void Init(float viewAngle, float aspect, float nearZ, float farZ);

		// ����p�̐ݒ�
		virtual void SetViewAngle(float viewAngle);

		// ���_�̐ݒ�
		virtual void SetPosition(const Vector3D& position);

		// ���_�̐ݒ�
		virtual void SetPosition(float positionX, float positionY, float positionZ);

		// ���_�̈ړ�
		virtual void MovePosition(const Vector3D& axis);

		// ���_�̈ړ�
		virtual void MovePosition(float axisX, float axisY, float axisZ);

		// ���_�̎擾
		const Vector3D& GetPosition()const{ return eye_; }

		// �����_�̐ݒ�
		virtual void SetFocus(const Vector3D& position);

		// �����_�̐ݒ�
		virtual void SetFocus(float positionX, float positionY, float positionZ);

		// �����_�̈ړ�
		virtual void MoveFocus(const Vector3D& axis);

		// �����_�̈ړ�
		virtual void MoveFocus(float axisX, float axisY, float axisZ);

		// �����_�̎擾
		const Vector3D& GetFocus()const{ return focus_; }

		// �r���[�|�[�g�̐ݒ�
		void SetViewPort(float topLeftX, float topLeftY, float width, float height);

		// ��ʑ̂̓o�^
		void AddObject(Object& object);

		// ��ʑ̂̓o�^����
		void RemoveObject(Object& object);

		// �S��ʑ̂̓o�^����
		void ResetObject();

		// ��ʑ̂̕`��
		void DrawObject();

		// �R���C�_�[�̔��a�̐ݒ�
		void SetColliderRadius(float radius){ radius_ = radius; }

		// �X�t�B�A�R���C�_�[�̎擾
		SphereCollider GetCollider(){ return sc_; }

		// �J�����Ɏʂ��Ă��邩
		bool IsVisible(const Object& object);

		// ���_�ƒ����_�̊Ԃɑ��݂��邩
		bool CheckHitRayToSphere(const Object& object);

		// �v���W�F�N�V�����}�g���N�X�̎擾
		const Matrix4x4& GetProjectionMatrix()const{ return proj_; }

		// �r���[�}�g���N�X�̎擾
		const Matrix4x4& GetViewMatrix()const{ return view_; }

		// �r���[�|�[�g�̎擾
		const ViewPort& GetViewPort()const{ return viewPort_; }

	protected:
		// ����p
		float viewAngle_ = 0.0f;

		// �A�X�y�N�g
		float aspect_ = 0.0f;

		// �j�A
		float nearZ_ = 0.0f;

		// �t�@�[
		float farZ_ = 0.0f;

		// ���_
		Vector3D eye_;

		// �����_
		Vector3D focus_;

		// �J�����̏�x�N�g��
		Vector3D up_;

		// �X�t�B�A�R���C�_�[
		SphereCollider sc_;

		// �v���W�F�N�V�����}�g���N�X
		Matrix4x4 proj_;

		// �r���[�}�g���N�X
		Matrix4x4 view_;

		// �r���[�|�[�g
		ViewPort viewPort_;

		// �R���W�����̔��a
		float radius_ = 0.0f;


		// ��ʑ̃��X�g
		std::vector<Object*> vecObjPtr_;

		// �r���[�}�g���N�X�̍X�V
		inline void UpdateViewMatrix();
	};
}

#endif	// #ifndef INCLUDE_IDEA_CAMERA_H