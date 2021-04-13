/*==============================================================================
	[ArchiveLoader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Archive/VirtualFileLoader.h"
#include "../../h/Archive/FileLoader.h"
#include "../../h/Archive/ArchiveData.h"
#include "../../h/Utility//ideaUtility.h"

using namespace idea;

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C���̓ǂݍ���
//------------------------------------------------------------------------------
bool ArchiveLoader::Load(const char* archiveFileName, const char* fileName)
{
	if(data_.size() != 0){ return false; }

	VirtualFileLoader loader;

	if(!loader.PushLoader(SFileLoader(ArchiveData::CreateInstance(archiveFileName)), "")){
		SetDebugMessage("ArchiveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", archiveFileName);
		return false;
	}

	loader.Load(data_, fileName);

	if(data_.size() == 0){ return false; }

	return true;
}

//------------------------------------------------------------------------------
// �f�[�^�̔j��
//------------------------------------------------------------------------------
void idea::ArchiveLoader::UnLoad()
{
	std::vector<BYTE>().swap(data_);
}
