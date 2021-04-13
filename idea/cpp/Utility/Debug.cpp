/*==============================================================================
	[Debug.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/Debug.h"
#include "../../h/Utility/DebugManager.h"
#include <stdarg.h>

using namespace idea;

bool Debug::IsDebugMode()
{
	return DebugManager::Instance().IsDebugMode();
}

bool  Debug::GetDebugSwitch(int index) const
{
	return DebugManager::Instance().GetDebugSwitch(index);
}

void Debug::SetDebugSwitch(int index, bool flag)
{
	DebugManager::Instance().SetDebugSwitch(index, flag);
}

//------------------------------------------------------------------------------
// ログの書き込み
//------------------------------------------------------------------------------
void Debug::Log(const char* pFormat, ...)
{
	va_list args;	// 可変個数の引数リスト

	// フォーマット指定された文字列を出力する
	va_start(args, pFormat);
	int len = _vscprintf(pFormat, args) + 1;
	std::vector<char> strBuffer;
	strBuffer.resize(len);
	vsprintf_s(&strBuffer[0], len, pFormat, args);
	va_end(args);

	std::string str(strBuffer.begin(), strBuffer.end());

	DebugManager::Instance().Log(str);
}

//------------------------------------------------------------------------------
// ログのクリア
//------------------------------------------------------------------------------
void Debug::ClearLog()
{
	DebugManager::Instance().ClearLog();
}

//------------------------------------------------------------------------------
// 変数のトレース
//------------------------------------------------------------------------------
void Debug::Trace(const char* pVariableName, int value)
{
	DebugManager::Instance().Trace(pVariableName, value);
}

void Debug::Trace(const char* pVariableName, float value)
{
	DebugManager::Instance().Trace(pVariableName, value);
}

void Debug::Trace(const char* pVariableName, const Vector2D& value)
{
	DebugManager::Instance().Trace(pVariableName, value);
}

void Debug::Trace(const char* pVariableName, const Vector3D& value)
{
	DebugManager::Instance().Trace(pVariableName, value);
}

void Debug::Trace(const char* pVariableName, const Vector4D& value)
{
	DebugManager::Instance().Trace(pVariableName, value);
}

//------------------------------------------------------------------------------
// トレース情報のクリア
//------------------------------------------------------------------------------
void Debug::ClearTrace()
{
	DebugManager::Instance().ClearTrace();
}