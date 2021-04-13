#pragma once

#include "../WinMain.h"

// ゲーム用描画クラス
class Renderer : public idea::GameObject{
public:
	Renderer();
	~Renderer();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// 描画
	void Draw()override;

	// カメラの取得
	idea::WrapCamera& GetWrapCamera();

	// 影描画用カメラの取得
	idea::ShadowCamera& GetShadowCamera();

	// 被写体の追加
	void AddObject(idea::Object& object);

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};