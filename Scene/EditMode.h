#pragma once

#include "../WinMain.h"

// マップ生成シーン
class EditMode : public idea::Scene{
public:
	EditMode();
	~EditMode();

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

	// オブジェクトの設置
	void SetObject(idea::Camera& camera);

	// ブロックの設置
	bool SetBlock(int& i, int& j, int count);

	// 坂の設置
	bool SetSlope(int& i, int& j, int count);

	// プレイヤーの設置
	void SetPlayer(int& i, int& j, int count);

	// AIの設置
	void SetVita(int& i, int& j, int count);

	// ブロックの消去
	bool RemoveBlock(idea::Camera& camera);

	// マップ情報の更新
	void UpdateMapData();

	// マップ情報の書き出し
	void WriteMapData();
};