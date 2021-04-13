#pragma once

#include "../WinMain.h"

// タッチした際のエフェクトのクラス
class TouchEffect : public idea::GameObject{
public:
	TouchEffect();
	~TouchEffect();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// 色の設定
	void SetColor(idea::Color color);

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};