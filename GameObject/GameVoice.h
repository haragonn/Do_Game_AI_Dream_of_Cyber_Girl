#pragma once

#include "../WinMain.h"

// キャラクター音声を管理するクラス
class GameVoice : public idea::GameObject{
public:
	GameVoice();
	~GameVoice();

	// 初期化
	void Init();

	// 更新
	void Update();

	// 開始ボイス
	void Start();

	// プレイヤータッチ
	void PlayerTouch();

	// vitaタッチ
	void VitaTouch();

	// vita増殖
	void AddVita(int vitaNum);

	// vita大量発生
	void FlockVita();

	// プレイヤー勝利
	void PlayerWin();

	// vita勝利
	void VitaWin();

private:
	// Pimplイディオム
	class Impl;

	// 実装
	Impl* pImpl_ = nullptr;

	// すべてのボイスを止める
	void StopAll();
};
