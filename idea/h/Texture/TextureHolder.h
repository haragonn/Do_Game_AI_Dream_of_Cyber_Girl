/*==============================================================================
	[TextureHolder.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTUREHOLDER_H
#define INCLUDE_IDEA_TEXTUREHOLDER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "Texture.h"

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FTextureHolder
	// �N���X�T�v�F�e�N�X�`���[�ێ��̍�,�e�N�X�`���\��delete�ɑΉ�������N���X
	//------------------------------------------------------------------------------
	class TextureHolder{
	protected:
		friend class Texture;

		Texture* pTexture_ = nullptr;

		TextureHolder() : pTexture_(nullptr){}	// �R���X�g���N�^
		virtual ~TextureHolder()	// �f�X�g���N�^
		{
			ExclusionTexture();
		}

		void SetTexture(Texture* pTexture)
		{
			ExclusionTexture();
			if(pTexture){
				pTexture_ = pTexture;
				auto it = find(pTexture_->vecTexHolderPtr_.begin(), pTexture_->vecTexHolderPtr_.end(), this);
				if(it == pTexture_->vecTexHolderPtr_.end()){
					pTexture_->vecTexHolderPtr_.push_back(this);
				}
			}
		}

		void ExclusionTexture()
		{
			if(pTexture_){
				auto it = find(pTexture_->vecTexHolderPtr_.begin(), pTexture_->vecTexHolderPtr_.end(), this);
				if(it != pTexture_->vecTexHolderPtr_.end()){
					pTexture_->vecTexHolderPtr_.erase(it);
				}
				pTexture_ = nullptr;
			}
		}
	};
}

#endif	// #ifndef INCLUDE_IDEA_TEXTUREHOLDER_H
