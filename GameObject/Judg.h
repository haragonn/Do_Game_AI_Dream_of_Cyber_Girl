#pragma once

#include "../WinMain.h"

namespace{
	// ゲーム時間
	static const int TIME_UP = 60 * 60;

	// 勝者不在
	static const int ABSENCE = 0;

	// プレイヤー勝利
	static const int PLAYER_WIN = 1;

	// vita勝利
	static const int VITA_WIN = 2;
}

// 勝敗判定クラス
class Judg : public idea::GameObject{
public:
	Judg();
	~Judg();

	// 初期化
	void Init()override;

	// 更新
	void Update()override;

	// プレイヤー逃走時間の取得
	int GetPlayerTime();

	// vita逃走時間の取得
	int GetVitaTime();

	// 勝者の取得
	int GetWinner();

	// プレイヤーがオニなった瞬間をセットする
	void SetPlayerOniFlag(bool flag);

	// プレイヤーはオニか
	bool IsPlayerOni();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;
};