#ifndef INCLUDE_IDEA_COLLISION_H
#define INCLUDE_IDEA_COLLISION_H

#include "../../h/Utility/ideaMath.h"

namespace idea{
	// �O���錾
	class BoxCollider;
	class SphereCollider;
	class CapsuleCollider;

	class BaseCollider{
	public:
		// �{�b�N�X�R���C�_�[�Ƃ̐ڐG����
		virtual bool CheckHit(const BoxCollider& bc) = 0;

		// �X�t�B�A�R���C�_�[�Ƃ̐ڐG����
		virtual bool CheckHit(const SphereCollider& sc) = 0;

		// �J�v�Z���R���C�_�[�Ƃ̐ڐG����
		virtual bool CheckHit(const CapsuleCollider& cc) = 0;

		// �_�Ƃ̍Őڋߓ_���擾����
		virtual Vector3D GetClosetPoint(const Vector3D& pt) = 0;

		// �߂荞�񂾃{�b�N�X�R���C�_�[���痣���
		virtual Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f) = 0;

		// �߂荞�񂾃X�t�B�A�R���C�_�[���痣���
		virtual Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f) = 0;

		// �߂荞�񂾃J�v�Z���R���C�_�[���痣���
		virtual Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f) = 0;
	};

	// �{�b�N�X�R���C�_�[(OBB)
	class BoxCollider : public BaseCollider{
	public:
		// OBB�̐ݒ�
		void SetOBB(const Matrix4x4& world, const Vector3D& prePosition);

		// �{�b�N�X�R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const BoxCollider& bc)override;

		// �X�t�B�A�R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const SphereCollider& sc)override;

		// �J�v�Z���R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const CapsuleCollider& cc)override;

		// �_�Ƃ̍Őڋߓ_���擾����
		Vector3D GetClosetPoint(const Vector3D& pt)override;

		// �߂荞�񂾃{�b�N�X�R���C�_�[���痣���
		Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f)override;

		// �߂荞�񂾃X�t�B�A�R���C�_�[���痣���
		Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f)override;

		// �߂荞�񂾃J�v�Z���R���C�_�[���痣���
		Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f)override;

		// �ʒu�̎擾
		const Vector3D& GetPosition()const{ return obb_.c; }

	private:
		friend class Collision3DHelper;

		// OBB
		struct OBB{
			Vector3D c;
			Vector3D u[3];
			Vector3D e;
		}obb_;

		// �O�̈ʒu
		Vector3D prePosition_;
	};

	// �X�t�B�A�R���C�_�[
	class SphereCollider : public BaseCollider{
	public:
		SphereCollider() : radius_(0.0f){}

		// �X�t�B�A�R���C�_�[�̐ݒ�
		void SetSphere(const Vector3D& position, float radius, const Vector3D& prePosition);

		// �{�b�N�X�R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const BoxCollider& bc)override;

		// �X�t�B�A�R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const SphereCollider& sc)override;

		// �J�v�Z���R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const CapsuleCollider& cc)override;

		// �_�Ƃ̍Őڋߓ_���擾����
		Vector3D GetClosetPoint(const Vector3D& pt)override;

		// �߂荞�񂾃{�b�N�X�R���C�_�[���痣���
		Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f)override;

		// �߂荞�񂾃X�t�B�A�R���C�_�[���痣���
		Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f)override;

		// �߂荞�񂾃J�v�Z���R���C�_�[���痣���
		Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f)override;

		// �ʒu�̎擾
		const Vector3D& GetPosition()const{ return position_; }

		// ���a�̎擾
		float GetRadius()const{ return radius_; }

	private:
		friend class Collision3DHelper;

		// �ʒu
		Vector3D position_;

		// �O�̈ʒu
		Vector3D prePosition_;

		// ���a
		float radius_ = 0.0f;
	};

	// �J�v�Z���R���C�_�[
	class CapsuleCollider : public BaseCollider{
	public:
		CapsuleCollider() : radius_(0.0f), height_(0.0f){}

		// �J�v�Z���R���C�_�[�̐ݒ�
		void SetCapsule(const Matrix4x4& world, float radius, float height, const Vector3D& prePosition);

		// �{�b�N�X�R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const BoxCollider& bc)override;

		// �X�t�B�A�R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const SphereCollider& sc)override;

		// �J�v�Z���R���C�_�[�Ƃ̐ڐG����
		bool CheckHit(const CapsuleCollider& cc)override;

		// �_�Ƃ̍Őڋߓ_���擾����
		Vector3D GetClosetPoint(const Vector3D& pt)override;

		// �߂荞�񂾃{�b�N�X�R���C�_�[���痣���
		Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f)override;

		// �߂荞�񂾃X�t�B�A�R���C�_�[���痣���
		Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f)override;

		// �߂荞�񂾃J�v�Z���R���C�_�[���痣���
		Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f)override;

		// �ʒu�̎擾
		const Vector3D& GetPosition()const{ return position_; }

		// ���a�̎擾
		float GetRadius()const{ return radius_; }

		// �����̎擾
		float GetHeight()const{ return height_; }

		// ����̎擾
		const Vector3D& GetTop()const{ return top_; }

		// �����̎擾
		const Vector3D& GetBottom()const{ return bottom_; }

	private:
		friend class Collision3DHelper;

		// �ʒu
		Vector3D position_;

		// �O�̈ʒu
		Vector3D prePosition_;

		// ���
		Vector3D top_;

		// ����
		Vector3D bottom_;

		// ��]�s��
		Matrix4x4 world_;

		// ���a
		float radius_ = 0.0f;

		// ����
		float height_ = 0.0f;
	};
}

#endif	// #ifndef INCLUDE_IDEA_COLLISION_H