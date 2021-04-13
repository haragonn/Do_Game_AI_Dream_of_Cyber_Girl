/*==============================================================================
	[Font.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FONT_H
#define INCLUDE_IDEA_FONT_H

#include "../Utility/ideaColor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace idea{
	// �t�H���g�̊�_
	enum class FontReferencePoint{
		UPPER_LEFT = 0,
		LEFT_CENTER,
		LOWER_LEFT,
		CENTER,
		UPPER_RIGHT,
		RIGHT_CENTER,
		LOWER_RIGHT,
		FONT_REFERENCE_POINT_MAX
	};

	//------------------------------------------------------------------------------
	// �N���X���@�FFont
	// �N���X�T�v�FAPI����t�H���g�̃e�N�X�`�����쐬��,�`�悷��N���X
	//------------------------------------------------------------------------------
	class Font{
	public:
		Font();
		~Font();

		// ������
		void Init(int size, int weight = FW_REGULAR, const FontReferencePoint point = FontReferencePoint::UPPER_LEFT, const char* pFontName = "�l�r �S�V�b�N");

		// �I������
		void UnInit();

		// �����̑傫���̎擾
		inline int GetSize()const{ return size_; }

		// �F�̐ݒ�
		void SetColor(const Color& color);

		// �F�̐ݒ�
		void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// �F�̎擾
		inline const Color& GetColor()const{ return color_; }

		// �����t���̕�����̕`��
		void DrawFormatText(float posX, float posY, const char* pFormat, ...);

	private:
		// �t�H���g�e�N�X�`�����
		typedef struct{
			ID3D11Texture2D* pFontTexture;
			ID3D11ShaderResourceView* pTextureView;
			int fontWidth;
			int fontHeight;
		}FontTextureData;

		// �t�H���g�n���h��
		HFONT hf_ = NULL;

		// �F
		Color color_;

		// �����̑傫��
		int size_ = 0;

		// ��_
		FontReferencePoint point_ = FontReferencePoint::UPPER_LEFT;

		// �t�H���g�e�N�X�`���̃��X�g
		std::unordered_map<UINT, FontTextureData> umTextureData_;

		// ��̃e�N�X�`�����
		static FontTextureData s_NullTextureData_;

		// �t�H���g�e�N�X�`���̐���
		FontTextureData* CreateTextureData(UINT code);

		// �L�[�R�[�h�ɑ΂���e�N�X�`����ێ����Ă��邩
		inline bool HasKeyTextureData(UINT code);

		// �R�s�[�R���X�g���N�^�̋֎~
		Font(const Font& src){}
		Font& operator=(const Font& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_FONT_H
