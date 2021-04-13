#pragma once

#include "../WinMain.h"

// タイトル用描画クラス
class TitleRenderer : public idea::GameObject{
public:
	TitleRenderer();
	~TitleRenderer();

	// 初期化
	void Init();

	// 描画
	void Draw();

	// カメラの取得
	idea::WrapCamera& GetWrapCamera();

	// 被写体の追加
	void AddObject(idea::Object& object);

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};