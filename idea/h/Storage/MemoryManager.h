/*==============================================================================
	[MemoryManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORYMANAGER_H
#define INCLUDE_IDEA_MEMORYMANAGER_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include <unordered_map>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：MemoryManager
	// クラス概要：シングルトン
	// 　　　　　　シーンをまたいで値を保持するクラス
	//------------------------------------------------------------------------------
	class MemoryManager{
	public:
		// 唯一のインスタンスを返す
		static MemoryManager& Instance(void)
		{
			static MemoryManager s_Instance;
			return s_Instance;
		}
		
		// 整数をキーに関連付け
		void SetInt(const char* pKey, int value);
		
		// 整数の取得
		int GetInt(const char* pKey);
		
		// 指定したキーと関連付けられた整数が存在するか
		bool HasKeyInt(const char* pKey);

		// 浮動小数点数をキーに関連付け
		void SetFloat(const char* pKey, float value);
		
		// 浮動小数点数の取得
		float GetFloat(const char* pKey);
		
		// 指定したキーと関連付けられた浮動小数点数が存在するか
		bool HasKeyFloat(const char* pKey);

		// 倍精度浮動小数点数をキーに関連付け
		void SetDouble(const char* pKey, double value);
		
		// 倍精度浮動小数点数の取得
		double GetDouble(const char* pKey);
		
		// 指定したキーと関連付けられた倍精度浮動小数点数が存在するか
		bool HasKeyDouble(const char* pKey);

	private:
		// 整数のマップ
		std::unordered_map<const char*, int> mapInt_;
		
		// 浮動小数点数のマップ
		std::unordered_map<const char*, float> mapFloat_;
		
		// 倍精度浮動小数点数のマップ
		std::unordered_map<const char*, double> mapDouble_;

		MemoryManager(){}
		~MemoryManager(){}

		// コピーコンストラクタの禁止
		MemoryManager(const MemoryManager& src){}
		MemoryManager& operator=(const MemoryManager& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_MEMORYMANAGER_H