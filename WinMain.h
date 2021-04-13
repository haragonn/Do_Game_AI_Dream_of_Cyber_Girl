/*==============================================================================
	[WinMain.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_WINMAIN_H
#define INCLUDE_WINMAIN_H

#include "idea/h/idea.h"

namespace{
	// スクリーンの幅
	static const int SCREEN_WIDTH = 1280;

	// スクリーンの高さ
	static const int SCREEN_HEIGHT = 720;

	// スクリーンの幅
	static const float S_W = SCREEN_WIDTH;

	// スクリーンの高さ
	static const float S_H = SCREEN_HEIGHT;

	// スクリーンの中央X座標
	static const float C_W = SCREEN_WIDTH * 0.5f;

	// スクリーンの中央Y座標
	static const float C_H = SCREEN_HEIGHT * 0.5f;
}

namespace{
	// 処理順レイヤー
	static const idea::GameObjectLayer MAP_LAYER = idea::GameObjectLayer::LAYER0;
	static const idea::GameObjectLayer PLAYER_LAYER = idea::GameObjectLayer::LAYER1;
	static const idea::GameObjectLayer VITA_LAYER = idea::GameObjectLayer::LAYER1;
	static const idea::GameObjectLayer PANEL_LAYER = idea::GameObjectLayer::LAYER1;
	static const idea::GameObjectLayer TITLE_CHARACTER_LAYER = idea::GameObjectLayer::LAYER1;
	static const idea::GameObjectLayer ITEM_LAYER = idea::GameObjectLayer::LAYER2;
	static const idea::GameObjectLayer BLOCK_LAYER = idea::GameObjectLayer::LAYER3;
	static const idea::GameObjectLayer COLLIDER_LAYER = idea::GameObjectLayer::LAYER4;
	static const idea::GameObjectLayer TOUCH_EFFECT_LAYER = idea::GameObjectLayer::LAYER4;
	static const idea::GameObjectLayer JUDG_LAYER = idea::GameObjectLayer::LAYER5;
	static const idea::GameObjectLayer META_AI_LAYER = idea::GameObjectLayer::LAYER5;
	static const idea::GameObjectLayer RENDERER_LAYER = idea::GameObjectLayer::LAYER6;
	static const idea::GameObjectLayer GAME_VOICE_LAYER = idea::GameObjectLayer::LAYER6;
	static const idea::GameObjectLayer EDIT_RENDERER_LAYER = idea::GameObjectLayer::LAYER6;
	static const idea::GameObjectLayer GAME_UI_LAYER = idea::GameObjectLayer::LAYER7;
	static const idea::GameObjectLayer TITLE_UI_LAYER = idea::GameObjectLayer::LAYER7;

	// ゲームモード
	static const int MODE_ID_MAIN_GAME = 0;
	static const int MODE_ID_EDIT_GAME = 1;
}
#endif	// #ifndef INCLUDE_WINMAIN_H