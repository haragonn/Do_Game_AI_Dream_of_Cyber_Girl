#pragma once

#include "../WinMain.h"

// タイトル画面のシーン
class Title : public idea::Scene{
public:
	Title();
	~Title();

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
	Impl* pImpl_;
};