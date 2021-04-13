#include "../../h/Storage/Storage.h"
#include "../../h/Storage/StorageManager.h"

using namespace idea;

//------------------------------------------------------------------------------
// 画像の読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadImageFromFile(const char* pFileName)
{
	return StorageManager::Instance().LoadImageFromFile(pFileName);
}

//------------------------------------------------------------------------------
// アーカイブファイルから画像を読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	return StorageManager::Instance().LoadImageFromArchiveFile(pArchiveFileName, pFileName);
}

//------------------------------------------------------------------------------
// 画像の破棄
// 引数　：ファイル名(const char* pFileName)
// 戻り値：なし
//------------------------------------------------------------------------------
void Storage::UnLoadImage(const char* pFileName)
{
	return StorageManager::Instance().SetUnLoadImageRequest(pFileName);
}

//------------------------------------------------------------------------------
// waveファイルの読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromFile(const char* pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromFile(pFileName, soundSize);
}

//------------------------------------------------------------------------------
// アーカイブファイルからwaveファイルを読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromArchiveFile(pArchiveFileName, pFileName, soundSize);
}

//------------------------------------------------------------------------------
// ソースボイスの破棄
// 引数　：ファイル名(const char* pFileName)
// 戻り値：なし
//------------------------------------------------------------------------------
void Storage::UnLoadWave(const char* pFileName)
{
	return StorageManager::Instance().SetUnLoadWaveRequest(pFileName);
}

bool Storage::LoadObjMeshFromFile(const char* pFileName)
{
	return StorageManager::Instance().LoadObjMeshFromFile(pFileName);
}

bool Storage::LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	return StorageManager::Instance().LoadObjMeshFromArchiveFile(pArchiveFileName, pFileName);
}

void Storage::UnLoadObjMesh(const char* pFileName)
{
	return StorageManager::Instance().SetUnLoadObjModelRequest(pFileName);
}

bool Storage::LoadPmxMeshFromFile(const char* pFileName)
{
	return StorageManager::Instance().LoadPmxMeshFromFile(pFileName);
}

bool Storage::LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	return StorageManager::Instance().LoadPmxMeshFromArchiveFile(pArchiveFileName, pFileName);
}

void Storage::UnLoadPmxMesh(const char* pFileName)
{
	StorageManager::Instance().SetUnLoadPmxModelRequest(pFileName);
}

bool Storage::LoadVmdMotionFromFile(const char* pFileName)
{
	return StorageManager::Instance().LoadVmdMotionFromFile(pFileName);
}

bool Storage::LoadVmdMotionFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	return StorageManager::Instance().LoadVmdMotionFromArchiveFile(pArchiveFileName, pFileName);
}

void Storage::UnLoadVmdMotion(const char* pFileName)
{
	StorageManager::Instance().SetUnLoadVmdMotionRequest(pFileName);
}

//------------------------------------------------------------------------------
// 全てのリソースの破棄
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void Storage::UnLoadAll()
{
	StorageManager::Instance().SetUnLoadAllRequest(true);
}
