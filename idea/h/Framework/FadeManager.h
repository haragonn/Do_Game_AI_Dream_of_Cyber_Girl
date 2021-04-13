/*==============================================================================
	[FadeManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADEMANAGER_H
#define INCLUDE_IDEA_FADEMANAGER_H

#include "../2D/Sprite.h"
#include "../Texture/Texture.h"

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：FadeManager
	// クラス概要：シングルトン
	// 　　　　　　フェードの設定,描画を行う
	//------------------------------------------------------------------------------
	class FadeManager{
	public:
		// 唯一のインスタンスを返す
		static FadeManager& Instance()
		{
			static FadeManager s_Instance;
			return s_Instance;
		}

		// 初期化
		void Init(float width, float height);
		
		// 終了処理
		void UnInit();

		// フェードの設定
		void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);

		// 更新
		void Update();
		
		// フェード中かどうか
		bool IsFading()const{ return !!(int)(a_ + 0.999999f); }
		
		// フェードアウトが完了したかどうか
		bool IsFadeOutCompleted()const{ return bFadeOutCompleted_; }

		// 描画
		void Draw();

	private:
		// スプライト
		Sprite spr_;

		// 赤成分
		float r_ = 0.0f;

		// 緑成分
		float g_ = 0.0f;

		// 青成分
		float b_ = 0.0f;

		// a値
		float a_ = 0.0f;

		// 変化量
		float value_ = 0.0f;

		// 有効かどうか
		bool bFadeEnable_ = false;
		
		// フェードアウトフラグ
		bool bFadeOut_ = false;
		
		// フェードアウト完了フラグ
		bool bFadeOutCompleted_ = false;

		FadeManager() :
			r_(1.0f),
			g_(1.0f),
			b_(1.0f),
			a_(0.0f),
			value_(1.0f),
			bFadeEnable_(false),
			bFadeOut_(false),
			bFadeOutCompleted_(false){}

		~FadeManager(){}

		// コピーコンストラクタの禁止
		FadeManager(const FadeManager& src){}
		FadeManager& operator =(const FadeManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_FADEMANAGER_H