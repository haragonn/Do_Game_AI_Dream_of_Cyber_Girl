/*==============================================================================
	[Debug.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_DEBUG_H
#define INCLUDE_IDEA_DEBUG_H

#include "../Utility/ideaMath.h"

#include <vector>
#include <string>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Debug
	// クラス概要：デバッグ機能集
	//------------------------------------------------------------------------------
	class Debug{
	public:
		// デバッグ表示中か
		bool IsDebugMode();

		// デバッグスイッチがオンか
		bool GetDebugSwitch(int index)const;

		// デバッグスイッチの切り替え
		void SetDebugSwitch(int index, bool flag);

		// ログの書き込み
		void Log(const char* pFormat, ...);

		// ログのクリア
		void ClearLog();

		// 変数のトレース
		void Trace(const char* pVariableName, int value);

		// 変数のトレース
		void Trace(const char* pVariableName, float value);

		// 変数のトレース
		void Trace(const char* pVariableName, const Vector2D& value);

		// 変数のトレース
		void Trace(const char* pVariableName, const Vector3D& value);

		// 変数のトレース
		void Trace(const char* pVariableName, const Vector4D& value);

		// トレース情報のクリア
		void ClearTrace();
	};
};

#endif	// #ifndef INCLUDE_IDEA_DEBUG_H