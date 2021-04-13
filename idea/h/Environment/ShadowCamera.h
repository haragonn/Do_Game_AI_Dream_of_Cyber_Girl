#ifndef INCLUDE_IDEA_SHADOWCAMERA_H
#define INCLUDE_IDEA_SHADOWCAMERA_H

#include "Camera.h"

namespace idea{
	class ShadowCamera : public Camera{
	public:
		ShadowCamera() : bShadowDraw_(false){}

		// ������
		void Init(float range, float nearZ, float farZ);

		// ���_�̐ݒ�
		void SetPosition(const Vector3D& position)override;

		// ���_�̐ݒ�
		void SetPosition(float positionX, float positionY, float positionZ)override;

		// ���_�̈ړ�
		void MovePosition(const Vector3D& axis)override;

		// ���_�̈ړ�
		void MovePosition(float axisX, float axisY, float axisZ)override;

		// �����_�̐ݒ�
		virtual void SetFocus(const Vector3D& position)override{}

		// �����_�̐ݒ�
		virtual void SetFocus(float positionX, float positionY, float positionZ)override{}

		// �����_�̈ړ�
		virtual void MoveFocus(const Vector3D& axis)override{}

		// �����_�̈ړ�
		virtual void MoveFocus(float axisX, float axisY, float axisZ)override{}

		// ��ʑ̂̕`��
		void DrawObject();

		// �e�`�ʒ��t���O�̎擾
		bool GetShadowDrawFlag()const{ return bShadowDraw_; }

	private:
		// ���W
		Vector3D position_;

		// �e�`��t���O
		bool bShadowDraw_ = false;

		// ������
		void Init(float viewAngle, float aspect, float nearZ, float farZ)override{}
	};
}

#endif	// #ifndef INCLUDE_IDEA_SHADOWCAMERA_H