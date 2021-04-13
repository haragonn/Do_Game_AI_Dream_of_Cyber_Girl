#pragma once

#include "../WinMain.h"

// マップおよびオブジェクトを生成するクラス
class Map : public idea::GameObject{
public:
	Map();
	~Map();

	// 初期化
	void Init()override;

	// 終了処理
	void UnInit()override;

	// 更新
	void Update()override;

	// ウェイポイントリストの取得
	idea::WayPoints& GetWayPoints();

	// マップの行の取得
	int GetRow();

	// マップの列の取得
	int GetCol();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;

	// 床の生成
	void CreatePlane();

	// スカイドームの生成
	void CreateSkyDome();

	// ブロックの生成
	void CreateBlock();

	// ノードの接続
	void ConnectNode();

	// 壁の生成
	void CreateWall();
};