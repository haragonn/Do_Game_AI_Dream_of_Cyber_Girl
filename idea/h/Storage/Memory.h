/*==============================================================================
	[Memory.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORY_H
#define INCLUDE_IDEA_MEMORY_H

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Memory
	// クラス概要：シーンをまたいで値を保持するクラス
	//------------------------------------------------------------------------------
	class Memory{
	public:
		// キーに整数を設定
		void SetInt(const char* pKey, int value);

		// 整数の取得
		int GetInt(const char* pKey, int def = 0);

		// 指定したキーと関連付けられた整数が存在するか
		bool HasKeyInt(const char* pKey);

		// キーに浮動小数点数を設定
		void SetFloat(const char* pKey, float value);

		// 浮動小数点数の取得
		float GetFloat(const char* pKey, float def = 0.0f);

		// 指定したキーと関連付けられた浮動小数点数が存在するか
		bool HasKeyFloat(const char* pKey);

		// キーに倍精度浮動小数点数を設定
		void SetDouble(const char* pKey, double value);

		// 倍精度浮動小数点数の取得
		double GetDouble(const char* pKey, double def = 0.0);

		// 指定したキーと関連付けられた倍精度浮動小数点数が存在するか
		bool HasKeyDouble(const char* pKey);
	};
}

#endif	// #ifndef INCLUDE_IDEA_MEMORY_H