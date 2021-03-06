/*==============================================================================
	[Timer.cpp]
														Author	:	Keigo Hara
==============================================================================*/
//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/Utility/Timer.h"
#pragma comment(lib, "winmm.lib")

using namespace idea;

//------------------------------------------------------------------------------
// 静的メンバ
//------------------------------------------------------------------------------
bool Timer::s_bSetPeriod_ = false;	// timeBeginPeriod()したフラグ

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Timer::Timer()
{
	// タイマーの精度を高める
	if(!s_bSetPeriod_){
		timeBeginPeriod(1);
		s_bSetPeriod_ = true;
	}

	// ゼロクリア
	ZeroMemory(&liStart_, sizeof(LARGE_INTEGER));
	ZeroMemory(&liNow_, sizeof(LARGE_INTEGER));
	ZeroMemory(&liFreq_, sizeof(LARGE_INTEGER));

	// 高分解能パフォーマンスカウンタが存在するか確認
	if(QueryPerformanceCounter(&liStart_)){
		bPerformanceCounter_ = true;
		QueryPerformanceFrequency(&liFreq_);
		clock_ = 1000.0 / liFreq_.QuadPart;	// 最大分解能から1クロックの単位を割り出す
	}else{ bPerformanceCounter_ = false; }
}

//------------------------------------------------------------------------------
// デストラクタ
//------------------------------------------------------------------------------
Timer::~Timer()
{
	// タイマーの精度を戻す
	if(s_bSetPeriod_){
		timeEndPeriod(1);
		s_bSetPeriod_ = false;
	}
}