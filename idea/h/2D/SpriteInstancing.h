/*==============================================================================
	[SpriteInstancing.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SPRITEINSTANCING_H
#define INCLUDE_IDEA_SPRITEINSTANCING_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// �O���錾
	//------------------------------------------------------------------------------
	class Sprite;
	class Texture;

	//------------------------------------------------------------------------------
	// �N���X���@�FSpriteInstancing
	// �N���X�T�v�FSprite���i�[��,��x�ɓ����摜��\������N���X
	//------------------------------------------------------------------------------
	class SpriteInstancing{
	public:
		SpriteInstancing();
		~SpriteInstancing();

		// ������
		void Init(const Vector2D& size);

		// ������
		void Init(float width, float height);

		// �I������
		void UnInit();

		// �T�C�Y�̐ݒ�
		void SetSize(const Vector2D& size);

		// �T�C�Y�̐ݒ�
		void SetSize(float width, float height);

		// �T�C�Y�̎擾
		inline const Vector2D& GetSize()const
		{
			return size_;
		}

		// ��]�p�̐ݒ�
		void SetRotation(float rad);

		// ��]
		void MoveRotation(float rad);

		// ��]�p�̎擾
		inline float GetRoteate()const
		{
			return radian_;
		}

		// �F�̐ݒ�
		void SetColor(const Color& color);

		// �F�̐ݒ�
		void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// �F�̎擾
		inline const Color& GetColor()const
		{
			return color_;
		}

		// �`�悷��X�v���C�g�̒ǉ�
		void AddSprite(Sprite& sprite);

		// �`�悷��X�v���C�g�̍폜
		void RemoveSprite(Sprite& sprite);

		// �`�悷��S�X�v���C�g���폜
		void ResetSprite();
	
		// ��`�̕`��
		void DrawRect(int blend = 0);

		// �e�N�X�`���̕`��
		void DrawTexture(const Texture& tex, int blend = 0);

		// ���������ꂽ�e�N�X�`���̕`��
		void DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend = 0);

		// �\���͈͂��w�肵���e�N�X�`���[�̕`��
		void DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend = 0);

	private:
		// �傫��
		Vector2D size_;

		// �p�x
		float radian_ = 0.0f;

		// �F
		Color color_;

		// ���]�t���O(U)
		bool bReversedU_ = false;

		// ���]�t���O(V)
		bool bReversedV_ = false;

		// �`�悷��X�v���C�g�̃��X�g
		std::vector<Sprite*> vecSpritePtr_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_SPRITEINSTANCING_H