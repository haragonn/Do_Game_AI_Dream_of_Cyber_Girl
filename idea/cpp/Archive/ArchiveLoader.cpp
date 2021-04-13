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
// アーカイブファイルの読み込み
//------------------------------------------------------------------------------
bool ArchiveLoader::Load(const char* archiveFileName, const char* fileName)
{
	if(data_.size() != 0){ return false; }

	VirtualFileLoader loader;

	if(!loader.PushLoader(SFileLoader(ArchiveData::CreateInstance(archiveFileName)), "")){
		SetDebugMessage("ArchiveLoadError! [%s] をファイルから読み込めませんでした\n", archiveFileName);
		return false;
	}

	loader.Load(data_, fileName);

	if(data_.size() == 0){ return false; }

	return true;
}

//------------------------------------------------------------------------------
// データの破棄
//------------------------------------------------------------------------------
void idea::ArchiveLoader::UnLoad()
{
	std::vector<BYTE>().swap(data_);
}
