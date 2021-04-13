/*==============================================================================
	[StorageManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/StorageManager.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Sound/Sound.h"
#include "../../h/Mesh/ObjModel.h"
#include "../../h/Mesh/PmxModel.h"
#include "../../h/Motion/VmdMotion.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;
using namespace idea;

StorageManager::StorageManager() :
	bUnLoadAll_(false),
	pNullTexture_(nullptr),
	pNullSound_(nullptr),
	pNullObjModel_(nullptr),
	pNullPmxModel_(nullptr),
	pNullVmdMotion_(nullptr){}

void StorageManager::CreatNullInstance()
{
	pNullTexture_ = new Texture;
	pNullSound_ = new Sound;
	pNullObjModel_ = new ObjModel;
	pNullPmxModel_ = new PmxModel;
	pNullVmdMotion_ = new VmdMotion;
}

void StorageManager::ReleaseNullInstance()
{
	SafeDelete(pNullTexture_);
	SafeDelete(pNullSound_);
	SafeDelete(pNullObjModel_);
	SafeDelete(pNullPmxModel_);
	SafeDelete(pNullVmdMotion_);
}

//------------------------------------------------------------------------------
// 画像の読み込み
// 引数　：ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadImageFromFile(const char * pFileName)
{
	if(HasKeyTexture(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	Texture* pTex = new Texture;
	bool bRes = pTex->LoadImageFromFile(pFileName);

	if(bRes){
		umTexturePtr_.insert(pair<const char*, Texture*>{ pFileName, pTex });
	} else{
		SafeDelete(pTex);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// アーカイブファイルから画像を読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadImageFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(HasKeyTexture(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	Texture* pTex = new Texture;
	bool bRes = pTex->LoadImageFromArchiveFile(pArchiveFileName, pFileName);

	if(bRes){
		umTexturePtr_.insert(pair<const char*, Texture*>{ pFileName, pTex });
	} else{
		SafeDelete(pTex);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// 画像の取得
// 引数　：ファイル名(const char* pFileName)
// 戻り値：テクスチャー
//------------------------------------------------------------------------------
Texture& StorageManager::GetTexture(const char * pFileName)
{
	if(!HasKeyTexture(pFileName)
		|| !umTexturePtr_.size()){
		SetDebugMessage("TexturLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadImageFromFile(pFileName)){
			return *pNullTexture_;
		}
	}

	Texture* pTex = umTexturePtr_[pFileName];

	if(!pTex){
		return *pNullTexture_;
	}

	return *pTex;
}

//------------------------------------------------------------------------------
// 指定したファイル名と関連付けられた画像が存在するか
// 引数　：ファイル名(const char* pFileName)
// 戻り値：存在するか
//------------------------------------------------------------------------------
bool StorageManager::HasKeyTexture(const char * pFileName)
{
	if(!umTexturePtr_.size()){ return false; }
	return (umTexturePtr_.find(pFileName) != umTexturePtr_.end());	// 検索
}

void StorageManager::SetUnLoadImageRequest(const char* pFileName)
{
	vector<const char*>::iterator it = vecUnLoadImageList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadImageList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadImageList_.push_back(pFileName);
}

void StorageManager::UnLoadImage()
{
	if(umTexturePtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadImageList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadImageList_.end();
		unordered_map<const char*, Texture*>::iterator itTexEnd = umTexturePtr_.end();
		for(; it != itEnd; ++it){
			if(umTexturePtr_.find(*it) != itTexEnd){
				unordered_map<const char*, Texture*>::iterator itTex = umTexturePtr_.find(*it);
				if(itTex->second){
					itTex->second->UnLoad();
					delete itTex->second;
					itTex->second = nullptr;
				}
				umTexturePtr_.erase(itTex);
			}
		}
	}
	vector<const char*>().swap(vecUnLoadImageList_);
}

//------------------------------------------------------------------------------
// waveファイルの読み込み
// 引数　：ファイル名(const char* pFileName),サウンド数(unsigned int soundSize)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadWaveFromFile(const char* pFileName, unsigned int soundSize)
{
	if(HasKeyWave(pFileName)
		|| soundSize == 0){ return false; }
	
	umSoundSize_.insert(pair<const char*, unsigned int >{ pFileName, soundSize });
	umSoundIndex_.insert(pair<const char*, unsigned int >{ pFileName, 0 });

	bool bRes;
	Sound* pSnd = new Sound[soundSize];
	for(unsigned int i = 0U; i < soundSize; ++i){
		bRes = pSnd[i].LoadWaveFromFile(pFileName);
	}

	if(bRes){
		umSoundPtr_.insert(pair<const char*, Sound*>{ pFileName, pSnd });
	} else{
		SafeDeleteArray(pSnd);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// アーカイブファイルからwaveファイルを読み込む
// 引数　：アーカイブファイル名(const char* pArchiveFileName),
// 　　　　ファイル名(const char* pFileName),サウンド数(unsigned int soundNum)
// 戻り値：成否
//------------------------------------------------------------------------------
bool StorageManager::LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize)
{
	if(HasKeyWave(pFileName)
		|| soundSize == 0){ return false; }

	umSoundSize_.insert(pair<const char*, unsigned int >{ pFileName, soundSize });
	umSoundIndex_.insert(pair<const char*, unsigned int >{ pFileName, 0 });

	bool bRes;
	Sound* pSnd = new Sound[soundSize];
	for(unsigned int i = 0U; i < soundSize; ++i){
		bRes = pSnd[i].LoadWaveFromArchiveFile(pArchiveFileName, pFileName);
	}

	if(bRes){
		umSoundPtr_.insert(pair<const char*, Sound*>{ pFileName, pSnd });
	} else{
		SafeDeleteArray(pSnd);
	}

	return bRes;
}

//------------------------------------------------------------------------------
// サウンドの取得
// 引数　：ファイル名(const char* pFileName)
// 戻り値：サウンド
//------------------------------------------------------------------------------
Sound& StorageManager::GetSound(const char * pFileName)
{
	if(!HasKeyWave(pFileName)
		|| !umSoundPtr_.size()){
		SetDebugMessage("WaveLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadWaveFromFile(pFileName)){
			return *pNullSound_;
		}
	}

	Sound* pSnd = umSoundPtr_[pFileName];

	if(!pSnd){
		return *pNullSound_;
	}

	Sound& snd = pSnd[umSoundIndex_[pFileName] % umSoundSize_[pFileName]];
	umSoundIndex_[pFileName] = umSoundIndex_[pFileName] + 1;

	return snd;
}

//------------------------------------------------------------------------------
// 指定したファイル名と関連付けられたwaveデータが存在するか
// 引数　：ファイル名(const char* pFileName)
// 戻り値：存在するか
//------------------------------------------------------------------------------
bool StorageManager::HasKeyWave(const char * pFileName)
{
	if(!umSoundPtr_.size()){ return false; }
	return (umSoundPtr_.find(pFileName) != umSoundPtr_.end());	// 検索
}

void StorageManager::SetUnLoadWaveRequest(const char * pFileName)
{
	vector<const char*>::iterator it = vecUnLoadWaveList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadWaveList_.end();
	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadWaveList_.push_back(pFileName);
}

void StorageManager::UnLoadWave()
{
	if(umSoundPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadWaveList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadWaveList_.end();
		unordered_map<const char*, Sound*>::iterator itSndEnd = umSoundPtr_.end();

		for(; it != itEnd; ++it){
			if(umSoundPtr_.find(*it) != itSndEnd){
				unordered_map<const char*, Sound*>::iterator itSnd = umSoundPtr_.find(*it);
				if(itSnd->second){
					for(unsigned int i = 0U; i < umSoundSize_[itSnd->first]; ++i){
						itSnd->second[i].UnLoad();
					}
					delete[] itSnd->second;
					itSnd->second = nullptr;
				}
				umSoundPtr_.erase(itSnd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadWaveList_);
}

bool StorageManager::LoadObjMeshFromFile(const char* pFileName)
{
	if(HasKeyObjModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	ObjModel* pMesh = new ObjModel;
	bool bRes = pMesh->LoadObjMeshFromFile(pFileName);

	if(bRes){
		umObjModelPtr_.insert(pair<const char*, ObjModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

bool StorageManager::LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	if(HasKeyObjModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	ObjModel* pMesh = new ObjModel;
	bool bRes = pMesh->LoadObjMeshFromArchiveFile(pArchiveFileName, pFileName);

	if(bRes){
		umObjModelPtr_.insert(pair<const char*, ObjModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

ObjModel& StorageManager::GetObjModel(const char* pFileName)
{
	if(!HasKeyObjModel(pFileName)
		|| !umObjModelPtr_.size()){
		SetDebugMessage("ObjMeshLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadObjMeshFromFile(pFileName)){
			return *pNullObjModel_;
		}
	}

	ObjModel* pObj = umObjModelPtr_[pFileName];

	if(!pObj){
		return *pNullObjModel_;
	}

	return *pObj;
}

bool StorageManager::HasKeyObjModel(const char* pFileName)
{
	if(!umObjModelPtr_.size()){ return false; }
	return (umObjModelPtr_.find(pFileName) != umObjModelPtr_.end());	// 検索
}

bool StorageManager::LoadPmxMeshFromFile(const char* pFileName)
{
	if(HasKeyPmxModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	PmxModel* pMesh = new PmxModel;
	bool bRes = pMesh->LoadPmxMeshFromFile(pFileName);

	if(bRes){
		umPmxModelPtr_.insert(pair<const char*, PmxModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

bool StorageManager::LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	if(HasKeyPmxModel(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	PmxModel* pMesh = new PmxModel;
	bool bRes = pMesh->LoadPmxMeshFromArchiveFile(pArchiveFileName, pFileName);

	if(bRes){
		umPmxModelPtr_.insert(pair<const char*, PmxModel*>{ pFileName, pMesh });
	} else{
		SafeDelete(pMesh);
	}

	return bRes;
}

PmxModel& StorageManager::GetPmxModel(const char* pFileName)
{
	if(!HasKeyPmxModel(pFileName)
		|| !umPmxModelPtr_.size()){
		SetDebugMessage("PmxMeshLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadPmxMeshFromFile(pFileName)){
			return *pNullPmxModel_;
		}
	}

	return *umPmxModelPtr_[pFileName];

	PmxModel* pPmx = umPmxModelPtr_[pFileName];

	if(!pPmx){
		return *pNullPmxModel_;
	}

	return *pPmx;
}

bool StorageManager::HasKeyPmxModel(const char* pFileName)
{
	if(!umPmxModelPtr_.size()){ return false; }
	return (umPmxModelPtr_.find(pFileName) != umPmxModelPtr_.end());	// 検索
}

bool StorageManager::LoadVmdMotionFromFile(const char* pFileName)
{
	if(HasKeyVmdMotion(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	VmdMotion* pMotion = new VmdMotion;
	bool bRes = pMotion->LoadVmdMotionFromFile(pFileName);

	if(bRes){
		umVmdMotionPtr_.insert(pair<const char*, VmdMotion*>{ pFileName, pMotion });
	} else{
		SafeDelete(pMotion);
	}

	return bRes;
}

bool StorageManager::LoadVmdMotionFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	//if(HasKeyVmdMotion(pFileName)){ return false; }	// 既にキーが登録されていれば終了

	//VmdMotion* pMotion = new VmdMotion;
	//bool bRes = pMotion->LoadVmdMotionFromFile(pArchiveFileName, pFileName);

	//if(bRes){
	//	umVmdMotionPtr_.insert(pair<const char*, VmdMotion*>{ pFileName, pMotion });
	//} else{
	//	SafeDelete(pMotion);
	//}

	//return bRes;

	return false;
}

VmdMotion& StorageManager::GetVmdMotion(const char* pFileName)
{
	if(!HasKeyVmdMotion(pFileName)
		|| !umVmdMotionPtr_.size()){
		SetDebugMessage("VmdMotionLoadError! [%s] はストレージに存在しません。ファイルから読み込みます。\n", pFileName);
		if(!LoadVmdMotionFromFile(pFileName)){
			return *pNullVmdMotion_;
		}
	}

	VmdMotion* pVmd = umVmdMotionPtr_[pFileName];

	if(!pVmd){
		return *pNullVmdMotion_;
	}

	return *pVmd;
}

bool StorageManager::HasKeyVmdMotion(const char* pFileName)
{
	if(!umVmdMotionPtr_.size()){ return false; }
	return (umVmdMotionPtr_.find(pFileName) != umVmdMotionPtr_.end());	// 検索
}

void StorageManager::SetUnLoadObjModelRequest(const char* pFileName)
{
	vector<const char*>::iterator it = vecUnLoadObjModelList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadObjModelList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadObjModelList_.push_back(pFileName);
}

void StorageManager::UnLoadObjModel()
{
	if(umObjModelPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadObjModelList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadObjModelList_.end();
		unordered_map<const char*, ObjModel*>::iterator itPmdEnd = umObjModelPtr_.end();

		for(; it != itEnd; ++it){
			if(umObjModelPtr_.find(*it) != itPmdEnd){
				unordered_map<const char*, ObjModel*>::iterator itPmd = umObjModelPtr_.find(*it);
				if(itPmd->second){
					itPmd->second->UnLoad();
					delete itPmd->second;
					itPmd->second = nullptr;
				}
				umObjModelPtr_.erase(itPmd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadObjModelList_);
}

void StorageManager::SetUnLoadPmxModelRequest(const char* pFileName)
{
	vector<const char*>::iterator it = vecUnLoadPmxModelList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadPmxModelList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadPmxModelList_.push_back(pFileName);
}

void StorageManager::UnLoadPmxModel()
{
	if(umPmxModelPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadPmxModelList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadPmxModelList_.end();
		unordered_map<const char*, PmxModel*>::iterator itPmxEnd = umPmxModelPtr_.end();

		for(; it != itEnd; ++it){
			if(umPmxModelPtr_.find(*it) != itPmxEnd){
				unordered_map<const char*, PmxModel*>::iterator itPmx = umPmxModelPtr_.find(*it);
				if(itPmx->second){
					itPmx->second->UnLoad();
					delete itPmx->second;
					itPmx->second = nullptr;
				}
				umPmxModelPtr_.erase(itPmx);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadPmxModelList_);
}

void StorageManager::SetUnLoadVmdMotionRequest(const char* pFileName)
{
	vector<const char*>::iterator it = vecUnLoadVmdMotionList_.begin();
	vector<const char*>::iterator itEnd = vecUnLoadVmdMotionList_.end();

	for(; it != itEnd; ++it){
		if(*it == pFileName){
			return;
		}
	}

	vecUnLoadVmdMotionList_.push_back(pFileName);
}

void StorageManager::UnLoadVmdMotion()
{
	if(umVmdMotionPtr_.size()){
		// イテレータを使って解放
		vector<const char*>::iterator it = vecUnLoadVmdMotionList_.begin();
		vector<const char*>::iterator itEnd = vecUnLoadVmdMotionList_.end();
		unordered_map<const char*, VmdMotion*>::iterator itVmdEnd = umVmdMotionPtr_.end();

		for(; it != itEnd; ++it){
			if(umVmdMotionPtr_.find(*it) != itVmdEnd){
				unordered_map<const char*, VmdMotion*>::iterator itVmd = umVmdMotionPtr_.find(*it);
				if(itVmd->second){
					itVmd->second->UnLoad();
					delete itVmd->second;
					itVmd->second = nullptr;
				}
				umVmdMotionPtr_.erase(itVmd);
			}
		}
	}

	vector<const char*>().swap(vecUnLoadPmxModelList_);
}

//------------------------------------------------------------------------------
// 全ての画像とサウンドの破棄
// 引数　：なし
// 戻り値：なし
//------------------------------------------------------------------------------
void StorageManager::UnLoadAll()
{
	// テクスチャ
	if(umTexturePtr_.size()){
		unordered_map<const char*, Texture*>::iterator itTex = umTexturePtr_.begin();
		unordered_map<const char*, Texture*>::iterator itTexEnd = umTexturePtr_.end();

		for(; itTex != itTexEnd; ++itTex){
			if(itTex->second){
				itTex->second->UnLoad();
				delete itTex->second;
				itTex->second = nullptr;
			}
		}
	}

	unordered_map<const char*, Texture*>().swap(umTexturePtr_);

	vector<const char*>().swap(vecUnLoadImageList_);

	// サウンド
	if(umSoundPtr_.size()){
		unordered_map<const char*, Sound*>::iterator itSnd = umSoundPtr_.begin();
		unordered_map<const char*, Sound*>::iterator itSndEnd = umSoundPtr_.end();

		for(; itSnd != itSndEnd; ++itSnd){
			if(itSnd->second){
				for(unsigned int i = 0U; i < umSoundSize_[itSnd->first]; ++i){
					itSnd->second[i].UnLoad();
				}
				delete[] itSnd->second;
				itSnd->second = nullptr;
			}
		}
	}

	unordered_map<const char*, Sound*>().swap(umSoundPtr_);
	unordered_map<const char*, unsigned int>().swap(umSoundSize_);
	unordered_map<const char*, unsigned int>().swap(umSoundIndex_);

	vector<const char*>().swap(vecUnLoadWaveList_);

	// OBJ
	if(umObjModelPtr_.size()){
		// イテレータを使って解放
		unordered_map<const char*, ObjModel*>::iterator itObj = umObjModelPtr_.begin();
		unordered_map<const char*, ObjModel*>::iterator itObjEnd = umObjModelPtr_.end();

		for(; itObj != itObjEnd; ++itObj){
			if(itObj->second){
				itObj->second->UnLoad();
				delete itObj->second;
				itObj->second = nullptr;
			}
		}
	}

	unordered_map<const char*, ObjModel*>().swap(umObjModelPtr_);

	vector<const char*>().swap(vecUnLoadObjModelList_);

	// PMX
	if(umPmxModelPtr_.size()){
		// イテレータを使って解放
		unordered_map<const char*, PmxModel*>::iterator itPmx = umPmxModelPtr_.begin();
		unordered_map<const char*, PmxModel*>::iterator itPmxEnd = umPmxModelPtr_.end();

		for(; itPmx != itPmxEnd; ++itPmx){
			if(itPmx->second){
				itPmx->second->UnLoad();
				delete itPmx->second;
				itPmx->second = nullptr;
			}
		}
	}
	unordered_map<const char*, PmxModel*>().swap(umPmxModelPtr_);

	vector<const char*>().swap(vecUnLoadPmxModelList_);

	// VMX
	if(umVmdMotionPtr_.size()){
		// イテレータを使って解放
		unordered_map<const char*, VmdMotion*>::iterator itVmd = umVmdMotionPtr_.begin();
		unordered_map<const char*, VmdMotion*>::iterator itVmdEnd = umVmdMotionPtr_.end();

		for(; itVmd != itVmdEnd; ++itVmd){
			if(itVmd->second){
				itVmd->second->UnLoad();
				delete itVmd->second;
				itVmd->second = nullptr;
			}
		}
	}
	unordered_map<const char*, VmdMotion*>().swap(umVmdMotionPtr_);

	vector<const char*>().swap(vecUnLoadVmdMotionList_);

	bUnLoadAll_ = false;
}