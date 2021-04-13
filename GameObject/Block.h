#pragma once

#include "../WinMain.h"

// ブロックの種類
enum class BlockStyle{
	Box,		// 箱
	UpperRight,	// 右上がりの坂
	UpperLeft,	// 左上がりの坂
	UpperFront,	// 手前上がりの坂
	UpperBack,	// 奥下がりの坂
	Wall1,		// 壁・横長
	Wall2,		// 壁・奥長
};

// マップに配置するブロックのクラス
class Block : public idea::GameObject{
public:
	Block();
	~Block();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// ブロックの種類の設定
	void SetStyle(BlockStyle style);

	// ブロックの種類の取得
	BlockStyle GetStyle();

	// 箱ポリゴンの取得
	idea::Cube& GetCube();

	// コリジョンの取得
	idea::BoxCollider GetCollider();

	// 色の設定
	void SetColor(idea::Color color);

	// セルの座標の設定
	void SetCell(int x, int y);

	// セルの座標の取得
	idea::Cell GetCell();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};