/*==============================================================================
	[TextureHolder.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TEXTUREHOLDER_H
#define INCLUDE_IDEA_TEXTUREHOLDER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "Texture.h"

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：TextureHolder
	// クラス概要：テクスチャー保持の際,テクスチャ―のdeleteに対応する基底クラス
	//------------------------------------------------------------------------------
	class TextureHolder{
	protected:
		friend class Texture;

		Texture* pTexture_ = nullptr;

		TextureHolder() : pTexture_(nullptr){}	// コンストラクタ
		virtual ~TextureHolder()	// デストラクタ
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
