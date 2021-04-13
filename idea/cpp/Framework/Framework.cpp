#include "../../h/Framework.h"
#include "../../h/Framework/FrameworkManager.h"
#include <stdarg.h>
#include <vector>

bool idea::Framework::Init(HINSTANCE hInst, int nCmdShow, const FrameworkInitDesc& fid)
{
	return FrameworkManager::Instance().Init(hInst, nCmdShow, fid.width, fid.height, fid.bWindowed, fid.fps, fid.frameSkipMax, fid.bNetwork, fid.bDebug, fid.pClassName);
}

bool idea::Framework::Init(HINSTANCE hInst, int nCmdShow, UINT width, UINT height, bool bWindowed, UINT fps, UINT frameSkipMax, bool bNetwork, bool bDebug, const char* pClassName)
{
	// idea起動
	return FrameworkManager::Instance().Init(hInst, nCmdShow, width, height, bWindowed, fps, frameSkipMax, bNetwork, bDebug, pClassName);
}

void idea::Framework::UnInit()
{
	FrameworkManager::Instance().UnInit();
}

void idea::Framework::Run(Scene* pInitScene)
{
	FrameworkManager::Instance().Run(pInitScene);
}

bool idea::Framework::SetWindowFormatText(const char* pFormat, ...)
{
	va_list args;	// 可変個数の引数リスト

	// フォーマット指定された文字列を出力する
	va_start(args, pFormat);
	int len = _vscprintf(pFormat, args) + 1;
	std::vector<char> strBuffer;
	strBuffer.resize(len);
	vsprintf_s(&strBuffer[0], len, pFormat, args);
	va_end(args);

	return FrameworkManager::Instance().SetWindowFormatText(&strBuffer[0]);
}

float idea::Framework::GetFps()
{
	return FrameworkManager::Instance().GetFps();
}

UINT idea::Framework::GetTargetFps()
{
	return FrameworkManager::Instance().GetTargetFps();
}

void idea::Framework::SetFpsSimpleDisplayFlag(bool bShow)
{
	FrameworkManager::Instance().SetFpsSimpleDisplayFlag(bShow);
}

double idea::Framework::GetUpdateTime()
{
	return FrameworkManager::Instance().GetUpdateTime();
}

void idea::Framework::SetMouseCursorVisibleFlag(bool bOnWindowed, bool bOnFullscreen)
{
	FrameworkManager::Instance().SetMouseCursorVisibleFlag(bOnWindowed, bOnFullscreen);
}
