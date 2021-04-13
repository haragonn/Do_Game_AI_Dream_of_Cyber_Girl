#pragma once

#include "../WinMain.h"

// 各種オブジェクトの当たり判定管理クラス
class Collider : public idea::GameObject{
public:
	// 初期化
	void Init(){}

	// 更新
	void Update();
};