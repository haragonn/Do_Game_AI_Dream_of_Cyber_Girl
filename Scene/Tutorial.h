#pragma once

#include "../WinMain.h"

// 操作説明のシーン
class Tutorial : public idea::Scene{
public:
	Tutorial();
	~Tutorial();

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