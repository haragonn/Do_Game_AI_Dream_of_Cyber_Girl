#include "../../h/Storage/Storage.h"
#include "../../h/Storage/StorageManager.h"

using namespace idea;

//------------------------------------------------------------------------------
// �摜�̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadImageFromFile(const char* pFileName)
{
	return StorageManager::Instance().LoadImageFromFile(pFileName);
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C������摜��ǂݍ���
// �����@�F�A�[�J�C�u�t�@�C����(const char* pArchiveFileName),
// �@�@�@�@�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	return StorageManager::Instance().LoadImageFromArchiveFile(pArchiveFileName, pFileName);
}

//------------------------------------------------------------------------------
// �摜�̔j��
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Storage::UnLoadImage(const char* pFileName)
{
	return StorageManager::Instance().SetUnLoadImageRequest(pFileName);
}

//------------------------------------------------------------------------------
// wave�t�@�C���̓ǂݍ���
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromFile(const char* pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromFile(pFileName, soundSize);
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C������wave�t�@�C����ǂݍ���
// �����@�F�A�[�J�C�u�t�@�C����(const char* pArchiveFileName),
// �@�@�@�@�t�@�C����(const char* pFileName)
// �߂�l�F����
//------------------------------------------------------------------------------
bool Storage::LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize)
{
	return StorageManager::Instance().LoadWaveFromArchiveFile(pArchiveFileName, pFileName, soundSize);
}

//------------------------------------------------------------------------------
// �\�[�X�{�C�X�̔j��
// �����@�F�t�@�C����(const char* pFileName)
// �߂�l�F�Ȃ�
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
// �S�Ẵ��\�[�X�̔j��
// �����@�F�Ȃ�
// �߂�l�F�Ȃ�
//------------------------------------------------------------------------------
void Storage::UnLoadAll()
{
	StorageManager::Instance().SetUnLoadAllRequest(true);
}
