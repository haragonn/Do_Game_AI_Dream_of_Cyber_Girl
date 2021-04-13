#pragma once

#include "../WinMain.h"

// モード選択画面のシーン
class Menu : public idea::Scene{
public:
	Menu();
	~Menu();

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
	Impl* pImpl_;
};