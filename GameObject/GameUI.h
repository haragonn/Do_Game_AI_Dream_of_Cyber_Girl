#pragma once

#include "../WinMain.h"

// ゲーム画面のUIを管理するクラス
class GameUI : public idea::GameObject{
public:
	GameUI();
	~GameUI();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// 描画
	void Draw()override;

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};
