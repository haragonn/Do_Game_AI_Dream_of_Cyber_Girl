/*==============================================================================
	[Timer.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TIMER_H
#define INCLUDE_IDEA_TIMER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Timer
	// クラス概要：時間計測を行うクラス
	//------------------------------------------------------------------------------
	class Timer{
	public:
		Timer();
		~Timer();

		// 計測開始時刻の設定
		void SetStartTime()
		{
			dwStart_ = timeGetTime();
			if(bPerformanceCounter_){ QueryPerformanceCounter(&liStart_); }
		}

		// 経過時間の取得
		unsigned long GetElapsed()
		{
			return static_cast<unsigned long>(timeGetTime() - dwStart_);
		}

		// 高精度な経過時間の取得
		double GetHighPrecisionElapsed()
		{
			if(bPerformanceCounter_){
				QueryPerformanceCounter(&liNow_);
				return (liNow_.QuadPart - liStart_.QuadPart) * clock_;
			}
			return static_cast<double>(timeGetTime() - dwStart_);
		}

	private:
		// timeBeginPeriod()したフラグ
		static bool s_bSetPeriod_;

		// 開始時刻
		DWORD dwStart_ = 0U;

		// 時刻
		LARGE_INTEGER liStart_ = {};
		LARGE_INTEGER liNow_ = {};
		LARGE_INTEGER liFreq_ = {};

		// 1クロック単位
		double clock_ = 0.0;

		// 高精度タイマーが存在するか
		bool bPerformanceCounter_ = false;
	};
}

#endif // #ifndef INCLUDE_IDEA_TIMER_H