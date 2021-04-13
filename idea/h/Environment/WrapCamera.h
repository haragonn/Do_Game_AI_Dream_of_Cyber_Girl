#ifndef INCLUDE_IDEA_WRAPCAMERA_H
#define INCLUDE_IDEA_WRAPCAMERA_H

#include "Camera.h"
#include "../../h/Utility/ideaMath.h"

namespace idea{
	class WrapCamera : public Camera{
	public:
		WrapCamera();
		~WrapCamera(){}

		// ������
		void Init(float viewAngle, float aspect, float nearZ, float farZ, float distance);

		// �����_�̐ݒ�
		void SetFocus(const Vector3D& position)override;

		// �����_�̐ݒ�
		void SetFocus(float positionX, float positionY, float positionZ)override;

		// �����_�̈ړ�
		void MoveFocus(const Vector3D& axis)override;

		// �����_�̈ړ�
		void MoveFocus(float axisX, float axisY, float axisZ)override;

		// �����_�Ƃ̋����̐ݒ�
		void SetDistance(float distance);

		// �����_�Ƃ̋����̈ړ�
		void MoveDistance(float aixs);

		// �����_�Ƃ̋����̎擾
		float GetDistance()const{ return distance_; }

		// �J�����A���O���̐ݒ�
		void SetRotation(Vector2D rot);

		// �J�����A���O���̐ݒ�
		void SetRotation(float yaw, float pitch);

		// �J�����A���O���̈ړ�
		void MoveRotation(Vector2D rot);

		// �J�����A���O���̈ړ�
		void MoveRotation(float yaw, float pitch);

		// �J�����A���O���̈ړ�
		void MoveRotation(Vector2D rot, float speed);

		// �J�����A���O���̈ړ�
		void MoveRotation(float yaw, float pitch, float speed);

		// �J�����A���O���̎擾
		float GetYaw(){ return rotation_.x; }

		// �J�����A���O���̎擾
		float GetPitch(){ return rotation_.y; }

		// �J�����A���O����臒l�̐ݒ�
		void SetPitchThreshold(Vector2D pitch);

		// �J�����A���O����臒l�̐ݒ�
		void SetPitchThreshold(float pitchMin, float pithMax);

		// ��ԖڕW�J�����A���O���̐ݒ�
		void SetWrapTarget(Vector2D rot);

		// ��ԖڕW�J�����A���O���̐ݒ�
		void SetWrapTarget(float yaw, float pitch);

		// �J�����A���O���̕��
		void UpdateWrap(float delta);

		// ��Ԓ���
		bool IsWrap()const{ return isWrap_; }

		// ��Ԃ̒��~
		void InterruptionWrap();

		// �J�������_����̃x�N�g���̕␳
		Vector2D FixVector(const Vector2D& vec);

	private:
		// �����_����̋���
		float distance_ = 0.0f;

		// �J�����A���O��
		Vector2D rotation_;

		// �J�����A���O���̎l����
		Quaternion qtn_;

		// �J�����A���O����臒l
		Vector2D pitchThreshold_;

		// ��ԊJ�n�J�����A���O��
		Quaternion startQtn_;

		// ��ԖڕW�J�����A���O��
		Quaternion targetQtn_;

		// ��Ԓ���
		bool isWrap_ = false;

		// ��ԌW��
		float t_ = 0.0f;

		// ���W�̍X�V
		inline void UpdatePosition();

		// ������
		void Init(float viewAngle, float aspect, float nearZ, float farZ)override{}

		// ���_�̐ݒ�
		void SetPosition(const Vector3D& position)override{}

		// ���_�̐ݒ�
		void SetPosition(float positionX, float positionY, float positionZ)override{}

		// ���_�̈ړ�
		void MovePosition(const Vector3D& axis)override{}

		// ���_�̈ړ�
		void MovePosition(float axisX, float axisY, float axisZ)override{}
	};
}

#endif	// #ifndef INCLUDE_IDEA_WRAPCAMERA_H