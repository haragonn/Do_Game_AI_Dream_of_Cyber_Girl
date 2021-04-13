/*==============================================================================
	[Actor2D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ACTOR2D_H
#define INCLUDE_IDEA_ACTOR2D_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FActor2D
	// �N���X�T�v�F2D��ł̈ʒu�Ƒ傫��,�p�x�����N���X
	// �@�@�@�@�@�@�R���W������t�H���[�@�\������
	//------------------------------------------------------------------------------
	class Actor2D{
	public:
		Actor2D();
		virtual ~Actor2D();

		// ������
		virtual void Init(const Vector2D& position, const Vector2D& size);

		// ������
		virtual void Init(float positionX, float positionY, const Vector2D& size);

		// ������
		virtual void Init(const Vector2D& position, float width, float height);

		// ������
		virtual void Init(float positionX, float positionY, float width, float height);

		// �I������
		virtual void UnInit();

		// ���W�̐ݒ�
		virtual void SetPosition(const Vector2D& position);

		// ���W�̐ݒ�
		virtual void SetPosition(float positionX, float positionY);

		// �O�t���[���̍��W���ƍ��W��ݒ�
		virtual void SetBasePosition(const Vector2D& position);

		// �O�t���[���̍��W���ƍ��W��ݒ�
		virtual void SetBasePosition(float positionX, float positionY);

		// ���W�̈ړ�
		virtual void MovePosition(const Vector2D& axis);

		// ���W�̈ړ�
		virtual void MovePosition(float axisX, float axisY);

		// ���W�̎擾
		inline const Vector2D& GetPosition()const
		{
			return position_;
		}

		// �O�t���[���̍��W�̎擾
		inline const Vector2D& GetPrePosition()const
		{
			return prePosition_;
		}

		// �傫���̐ݒ�
		virtual void SetSize(const Vector2D& size);

		// �傫���̐ݒ�
		virtual void SetSize(float width, float height);

		// �傫���̎擾
		inline const Vector2D& GetSize()const
		{
			return size_;
		}

		// ��]�p�̐ݒ�
		virtual void SetRotation(float radian);

		// ��]
		virtual void MoveRotation(float radian);

		// ��]�p�̎擾
		inline float GetRoteate()const
		{
			return radian_;
		}

		// �F�̐ݒ�
		virtual void SetColor(const Color& color);

		// �F�̐ݒ�
		virtual void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// �F�̎擾
		inline const Color& GetColor()const
		{
			return color_;
		}

		// �~�̐ڐG����
		bool CheckHitCircle(const Actor2D& target)const;

		// ��`�̐ڐG����
		bool CheckHitRect(const Actor2D& target)const;

		// �ʒu�֌W�擾
		float GetRelation(const Actor2D& target)const;

		// �O�t���[���̈ʒu�֌W�擾
		float GetPreFrameRelation(const Actor2D& target)const;

		// �Ǐ]����^�[�Q�b�g�̐ݒ�
		virtual void FollowActor(Actor2D& target);

		// �t�H�����[����߂�
		virtual void QuitFollower();

	protected:
		// ���W
		Vector2D position_;

		// �O�t���[���̍��W
		Vector2D prePosition_;

		// �傫��
		Vector2D size_;

		// �p�x
		float radian_ = 0.0f;

		// ���]�t���O(U)
		bool bReversedU_ = false;

		// ���]�t���O(V)
		bool bReversedV_ = false;

		// �F
		Color color_;

	private:
		friend class Actor2DManager;

		// �o�^�t���O
		bool bRegistered_ = false;

		// ���[�_�[�̃|�C���^�[
		Actor2D* pLeader_ = nullptr;

		// �t�H�����[�̃|�C���^�[
		Actor2D* pFollower_ = nullptr;

		// �t�H�����[�̈ʒu���ړ�
		void MoveFollower(Vector2D axis);

		// �t�H�����[�̉��U
		void ResetFollower(Actor2D* pTarget);

		// ���W�̕ێ�
		void UpdatePrePosition();
	};
}

#endif	// #ifndef INCLUDE_IDEA_ACTOR2D_H