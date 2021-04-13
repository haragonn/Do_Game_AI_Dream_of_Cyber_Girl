/*==============================================================================
	[Memory.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/Memory.h"
#include "../../h/Storage/MemoryManager.h"

using namespace idea;


//------------------------------------------------------------------------------
// キーに整数を設定
//------------------------------------------------------------------------------
void Memory::SetInt(const char* pKey, int value)
{
	MemoryManager::Instance().SetInt(pKey, value);
}

//------------------------------------------------------------------------------
// 整数の取得
//------------------------------------------------------------------------------
int Memory::GetInt(const char* pKey, int def)
{
	if(!HasKeyInt(pKey)){ return def; }
	return MemoryManager::Instance().GetInt(pKey);
}

//------------------------------------------------------------------------------
// 指定したキーと関連付けられた整数が存在するか
//------------------------------------------------------------------------------
bool Memory::HasKeyInt(const char* pKey)
{
	return MemoryManager::Instance().HasKeyInt(pKey);
}

//------------------------------------------------------------------------------
// キーに浮動小数点数を設定
//------------------------------------------------------------------------------
void Memory::SetFloat(const char* pKey, float value)
{
	MemoryManager::Instance().SetFloat(pKey, value);
}

//------------------------------------------------------------------------------
// 浮動小数点数の取得
//------------------------------------------------------------------------------
float Memory::GetFloat(const char* pKey, float def)
{
	if(!HasKeyFloat(pKey)){ return def; }
	return MemoryManager::Instance().GetFloat(pKey);
}

//------------------------------------------------------------------------------
// 指定したキーと関連付けられた浮動小数点数が存在するか
//------------------------------------------------------------------------------
bool Memory::HasKeyFloat(const char* pKey)
{
	return MemoryManager::Instance().HasKeyFloat(pKey);
}

//------------------------------------------------------------------------------
// キーに倍精度浮動小数点数を設定
//------------------------------------------------------------------------------
void Memory::SetDouble(const char* pKey, double value)
{
	MemoryManager::Instance().SetDouble(pKey, value);
}

//------------------------------------------------------------------------------
// 倍精度浮動小数点数の取得
//------------------------------------------------------------------------------
double Memory::GetDouble(const char* pKey, double def)
{
	if(!HasKeyFloat(pKey)){ return def; }
	return MemoryManager::Instance().GetDouble(pKey);
}

//------------------------------------------------------------------------------
// 指定したキーと関連付けられた倍精度浮動小数点数が存在するか
//------------------------------------------------------------------------------
bool Memory::HasKeyDouble(const char* pKey)
{
	return MemoryManager::Instance().HasKeyDouble(pKey);
}
