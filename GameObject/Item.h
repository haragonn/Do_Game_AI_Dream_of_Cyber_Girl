#pragma once

#include "../WinMain.h"

// アイテムクラス
class Item : public idea::GameObject{
public:
	Item();
	~Item();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// 衝突時反応
	void HitObstacle(const idea::Vector3D& axis);

	// セルの設定
	void SetCell(int x, int y);

	// セルの取得
	idea::Cell& GetCell();

	// コリジョンの取得
	idea::SphereCollider GetCollider();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};