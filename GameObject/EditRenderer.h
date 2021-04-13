#pragma once

#include "../WinMain.h"

// エディットモード用のレンダラークラス
class EditRenderer : public idea::GameObject{
public:
	EditRenderer();
	~EditRenderer();

	// 初期化
	void Init();

	// 更新
	void Update();

	// 描画
	void Draw();

	// カメラの取得
	idea::WrapCamera& GetWrapCamera();

	// 被写体の追加
	void AddObject(idea::Object& object);

private:
	// Pimplイディオム
	class impl;

	// 実装
	impl* pImpl_ = nullptr;
};