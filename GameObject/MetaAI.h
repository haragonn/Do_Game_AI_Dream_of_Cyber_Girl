#pragma once

#include "../WinMain.h"

// ゲームレベルを管理するクラス
class MetaAI : public idea::GameObject{
public:
	MetaAI();
	~MetaAI();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// プレイヤースキルの評価
	void EvaluationPlayerSkill();

	// 難易度調整
	void AdjustmentDifficulty();

	// AIレベル調整
	void AdjustmentVitaLevel();

	// AIのセット
	void SetVita();

	// AIの追加
	void AddVita();

	// アイテムのセット
	void SetItem();

	// デバッグ情報の表示
	void ShowDebug();

	// vitaの待機範囲の取得
	float GetVitaIdolRange();

	// vitaの近づく速度の取得
	float GetVitaStopBySpeed();

	// vitaの追いかける速度の取得
	float GetVitaChaseSpeed();

	// vitaの逃げる速度の取得
	float GetVitaEscapeSpeed();

	// vitaの坂を上る速度のレートの取得
	float GetVitaClimbRate();

	// オニが切り替わったことを知らせる
	void OniChange();

	// アイテムが取得されたことを知らせる
	void GetItem();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};