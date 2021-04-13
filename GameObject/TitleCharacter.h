#pragma once

#include "../WinMain.h"

namespace{
	// 状態変化時間
	static const int STATE_TIME = 180;
}

// キャラクターの状態
enum class TitleCharacterState{
	Idol,	// 待機
	Chase,	// 追いかける
	Escape	// 逃げる
};

// タイトルで表示されるキャラクター達を管理するクラス
class TitleCharacter : public idea::StateMachine<TitleCharacterState>{
public:
	TitleCharacter();
	~TitleCharacter();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};