#pragma once

#include "../WinMain.h"

// プレイヤーの状態
enum class PlayerState{
	Idol,	// 待機
	Walk,	// 移動
	Back,	// 吹き飛ばされる
	Count	// カウント
};

// プレイヤーキャラクターのクラス
class Player : public idea::StateMachine<PlayerState>{
public:
	Player();
	~Player();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// カウント状態へ移行
	void Count();

	// 障害物に合った際のリアクション
	void HitObstacle(const idea::Vector3D& axis, const idea::Vector3D& back);

	// アイテムの取得時のリアクション
	void Doping();

	// セルの設定
	void SetCell(int x, int y);

	// セルの取得
	idea::Cell& GetCell();

	// コライダーの取得
	idea::CapsuleCollider GetCollider();

	// スタミナ残量の取得
	float GetStamina();

	// 疲労状態時間の取得
	int GetExhausted();

	// アイテム加速状態時間の取得
	int GetDoping();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;

	// 注目するvitaの決定
	void LookOnVita();
};