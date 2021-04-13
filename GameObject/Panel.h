#pragma once

#include "../WinMain.h"

// マウスポインタに反応するパネルクラス
class Panel : public idea::GameObject{
public:
	Panel();
	~Panel();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// セルの設定
	void SetCell(int x, int y);

	// 色の設定
	void SetColor(idea::Color color);

	// 箱の取得
	idea::Cube& GetCube();

	// コライダーの取得
	idea::BoxCollider GetCollider();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};