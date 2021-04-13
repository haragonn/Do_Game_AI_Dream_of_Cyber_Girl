#pragma once

#include "../WinMain.h"

// タイトルのUIを管理するクラス
class TitleUI : public idea::GameObject{
public:
	TitleUI();
	~TitleUI();

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