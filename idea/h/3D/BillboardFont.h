#ifndef INCLUDE_IDEA_BILLBOARDFONT_H
#define INCLUDE_IDEA_BILLBOARDFONT_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Object.h"
#include "Billboard.h"
#include "../Texture/Texture.h"
#include "../Utility/ideaColor.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unordered_map>

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;

namespace idea{
	class Camera;

	class BillboardFont{
	public:
		BillboardFont();
		~BillboardFont();

		// ������
		void Init(const idea::Vector3D& position, Camera& camera, int size, int weight = FW_REGULAR, const char* pFontName = "�l�r �S�V�b�N");

		// �I������
		void UnInit();

		// ������
		void SetPosition(const Vector3D& position);

		// ������
		void SetPosition(float positionX, float positionY, float positionZ);

		// �F�̐ݒ�
		virtual void SetColor(const Color& color);

		// �F�̐ݒ�
		virtual void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		//Y�r���{�[�h�t���O�̐ݒ�
		void SetYBillboardFlag(bool flag);

		// �����t���̕�����̕`��
		void DrawFormatText(const char* pFormat, ...);

	private:
		// �t�H���g�e�N�X�`�����
		struct BillboardFontTextureData{
			ID3D11Texture2D* pFontTexture = nullptr;
			ID3D11ShaderResourceView* pTextureView = nullptr;
			int fontWidth = 0;
			int fontHeight = 0;
		};

		// �t�H���g�n���h��
		HFONT hf_ = NULL;

		int size_ = 0;

		// ���W
		idea::Vector3D position_;

		// �F
		Color color_;

		// Y�r���{�[�h�t���O
		bool bYBillboard_ = false;

		Billboard billboard_;

		bool bAddFlag_ = false;

		// �t�H���g�e�N�X�`���̃��X�g
		std::unordered_map<UINT, BillboardFontTextureData> umTextureData_;

		Texture texture_;

		// ��̃e�N�X�`�����
		static BillboardFontTextureData s_NullTextureData_;

		// ��̃e�N�X�`�����
		static Texture pNullTexture_;

		// �t�H���g�e�N�X�`���̐���
		BillboardFontTextureData* CreateTextureData(UINT code);

		// �L�[�R�[�h�ɑ΂���e�N�X�`����ێ����Ă��邩
		inline bool HasKeyTextureData(UINT code);

		// �R�s�[�R���X�g���N�^�̋֎~
		BillboardFont(const BillboardFont& src){}
		BillboardFont& operator=(const BillboardFont& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_BILLBOARDFONT_H
