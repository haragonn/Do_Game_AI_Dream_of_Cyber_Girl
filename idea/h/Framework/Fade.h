/*==============================================================================
	[Fade.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_FADE_H
#define INCLUDE_IDEA_FADE_H

#include "../Utility/ideaColor.h"

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Fade
	// クラス概要：フェードの管理を行うクラス
	//------------------------------------------------------------------------------
	class Fade{
	public:
		// フェードの設定
		void SetFade(int time, const Color& color);
		
		// フェードの設定
		void SetFade(int time, float r = 1.0f, float g = 1.0f, float b = 1.0f);
		
		// フェードインアウト中か
		bool IsFading();
		
		// フェードアウトが完了したか
		bool IsFadeOutCompleted();
	};
}
#endif	// #ifndef INCLUDE_IDEA_FADE_H