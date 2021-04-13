#pragma once

#include "../WinMain.h"

// vitaの状態
enum class VitaState{
	Idol,			// 待機
	StopBy,			// 近づく
	Chase,			// 追いかける
	Escape,			// 逃げる
	Withdrawal,		// 離脱
	Count,			// カウント
	Appearance,		// 登場
	Disappearance	// 消滅
};

// vitaの役割
enum class VitaRole{
	Main,		// 本体
	Chase,		// 追いかける
	Wrap,		// 先回り
	Ambush		// 待ち伏せ
};

// AIキャラクターのクラス(ソース内ではモデルの名前からとってvitaと呼称する)
class Vita : public idea::StateMachine<VitaState>{
public:
	Vita();
	~Vita();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// 障害物に合った際のリアクション
	void HitObstacle(const idea::Vector3D& axis, const idea::Vector3D& back);

	// セルの設定
	void SetCell(int x, int y);

	// セルの取得
	idea::Cell& GetCell();

	// コライダーの取得
	idea::CapsuleCollider GetCollider();

	// 役割の設定
	void SetRole(VitaRole role);

	// 役割の設定
	VitaRole GetRole();

	// 追いかける状態へ移行
	void Chase();

	// 待機状態へ移行
	void Idol();

	// 逃げる状態へ移行
	void Escape();

	// カウント状態へ移行
	void Count();

	// 登場状態へ移行
	void Appearance();

	// 消滅状態へ移行
	void Disappearance();

	// 逃げる目標を取得
	idea::Vector3D GetEscapePoint();

	// 追いかける速度の調整
	void SetChaseSpeedRate(float rate);

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};