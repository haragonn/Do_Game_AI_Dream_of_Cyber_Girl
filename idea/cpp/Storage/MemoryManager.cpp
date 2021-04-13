/*==============================================================================
	[MemoryManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/MemoryManager.h"

using namespace idea;

//------------------------------------------------------------------------------
// 整数をキーに関連付け
//------------------------------------------------------------------------------
void MemoryManager::SetInt(const char* pKey, int value)
{
	if(HasKeyInt(pKey)){	// 既にキーが登録されていれば
		mapInt_[pKey] = value;	// 値を更新
		return;
	}
	mapInt_.insert(std::unordered_map<const char*, int>::value_type(pKey, value));	// 新規登録
}

//------------------------------------------------------------------------------
// 整数の取得
//------------------------------------------------------------------------------
int MemoryManager::GetInt(const char* pKey)
{
	if(!HasKeyInt(pKey)){ return 0; }	// キーが存在しなければ0を返す
	return mapInt_[pKey];	// 値を返す
}

//------------------------------------------------------------------------------
// 指定したキーと関連付けられた整数が存在するか
//------------------------------------------------------------------------------
bool MemoryManager::HasKeyInt(const char* pKey)
{
	return (mapInt_.find(pKey) != mapInt_.end());	// 検索
}

//------------------------------------------------------------------------------
// 浮動小数点数をキーに関連付け
//------------------------------------------------------------------------------
void MemoryManager::SetFloat(const char* pKey, float value)
{
	if(HasKeyFloat(pKey)){	// 既にキーが登録されていれば
		mapFloat_[pKey] = value;	// 値を更新
		return;
	}
	mapFloat_.insert(std::unordered_map<const char*, float>::value_type(pKey, value));	// 新規登録
}

//------------------------------------------------------------------------------
// 浮動小数点数の取得
//------------------------------------------------------------------------------
float MemoryManager::GetFloat(const char* pKey)
{
	if(!HasKeyFloat(pKey)){ return 0.0f; }	// キーが存在しなければ0.0fを返す
	return mapFloat_[pKey];	// 値を返す
}

//------------------------------------------------------------------------------
// 指定したキーと関連付けられた浮動小数点数が存在するか
//------------------------------------------------------------------------------
bool MemoryManager::HasKeyFloat(const char* pKey)
{
	return (mapFloat_.find(pKey) != mapFloat_.end());	// 検索
}

//------------------------------------------------------------------------------
// 倍精度浮動小数点数をキーに関連付け
//------------------------------------------------------------------------------
void MemoryManager::SetDouble(const char* pKey, double value)
{
	if(HasKeyDouble(pKey)){	// 既にキーが登録されていれば
		mapDouble_[pKey] = value;	// 値を更新
		return;
	}
	mapDouble_.insert(std::unordered_map<const char*, double>::value_type(pKey, value));	// 新規登録
}

//------------------------------------------------------------------------------
// 倍精度浮動小数点数の取得
//------------------------------------------------------------------------------
double MemoryManager::GetDouble(const char* pKey)
{
	if(!HasKeyDouble(pKey)){ return 0.0; }	// キーが存在しなければ0.0を返す
	return mapDouble_[pKey];	// 値を返す
}

//------------------------------------------------------------------------------
// 指定したキーと関連付けられた倍精度浮動小数点数が存在するか
//------------------------------------------------------------------------------
bool MemoryManager::HasKeyDouble(const char* pKey)
{
	return (mapDouble_.find(pKey) != mapDouble_.end());	// 検索
}