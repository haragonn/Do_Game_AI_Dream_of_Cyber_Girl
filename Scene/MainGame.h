#pragma once

#include "../WinMain.h"

// 既存ステージで遊ぶシーン
class MainGame : public idea::Scene{
public:
	MainGame();
	~MainGame();

	// 初期化
	void Init()override;

	// 終了処理
	void UnInit()override;

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
