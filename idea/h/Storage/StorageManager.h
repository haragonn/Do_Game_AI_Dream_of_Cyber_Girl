/*==============================================================================
	[StorageManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_STORAGEMANAGER_H
#define INCLUDE_IDEA_STORAGEMANAGER_H

#include <vector>
#include <unordered_map>

namespace idea{
	class Texture;
	class Sound;
	class ObjModel;
	class PmxModel;
	class VmdMotion;

	//------------------------------------------------------------------------------
	// クラス名　：MemoryManager
	// クラス概要：シングルトン
	// 　　　　　　リソースを読み込み,シーンをまたいでリソースを保持するクラス
	//------------------------------------------------------------------------------
	class StorageManager{
	public:
		// 唯一のインスタンスを返す
		static StorageManager& Instance()
		{
			static StorageManager s_Instance;
			return s_Instance;
		}

		void CreatNullInstance();

		void ReleaseNullInstance();

		// 画像の読み込み
		bool LoadImageFromFile(const char* pFileName);
		
		// アーカイブファイルから画像を読み込む
		bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);
		
		// 画像の取得
		Texture& GetTexture(const char* pFileName);
		
		// 指定したファイル名と関連付けられた画像が存在するか
		bool HasKeyTexture(const char* pFileName);

		// waveファイルからサウンドを読み込む
		bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);
		
		// アーカイブファイルの中のwaveファイルからサウンドを読み込む
		bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);

		// サウンドの取得
		Sound& GetSound(const char* pFileName);

		// 指定したファイル名と関連付けられたサウンドが存在するか
		bool HasKeyWave(const char* pFileName);

		void SetUnLoadImageRequest(const char* pFileName);
		inline int GetUnLoadImageRequestSize()const{ return vecUnLoadImageList_.size(); }
		void UnLoadImage();

		void SetUnLoadWaveRequest(const char* pFileName);
		inline int GetUnLoadWaveRequestSize()const{ return vecUnLoadWaveList_.size(); }
		void UnLoadWave();


		bool LoadObjMeshFromFile(const char* pFileName);	// 画像の読み込み
		bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルから画像を読み込む
		ObjModel& GetObjModel(const char* pFileName);	// 画像の取得
		bool HasKeyObjModel(const char* pFileName);	// 指定したファイル名と関連付けられた画像が存在するか

		bool LoadPmxMeshFromFile(const char* pFileName);	// 画像の読み込み
		bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルから画像を読み込む
		PmxModel& GetPmxModel(const char* pFileName);	// 画像の取得
		bool HasKeyPmxModel(const char* pFileName);	// 指定したファイル名と関連付けられた画像が存在するか

		bool LoadVmdMotionFromFile(const char* pFileName);	// 画像の読み込み
		bool LoadVmdMotionFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// アーカイブファイルから画像を読み込む
		VmdMotion& GetVmdMotion(const char* pFileName);	// 画像の取得
		bool HasKeyVmdMotion(const char* pFileName);	// 指定したファイル名と関連付けられた画像が存在するか

		void SetUnLoadObjModelRequest(const char* pFileName);
		inline int GetUnLoadObjModelRequestSize()const{ return vecUnLoadObjModelList_.size(); }
		void UnLoadObjModel();

		void SetUnLoadPmxModelRequest(const char* pFileName);
		inline int GetUnLoadPmxModelRequestSize()const{ return vecUnLoadPmxModelList_.size(); }
		void UnLoadPmxModel();

		void SetUnLoadVmdMotionRequest(const char* pFileName);
		inline int GetUnLoadVmdMotionRequestSize()const{ return vecUnLoadVmdMotionList_.size(); }
		void UnLoadVmdMotion();

		inline void SetUnLoadAllRequest(bool bRequest){ bUnLoadAll_ = bRequest; }
		inline bool IsUnLoadAllRequest()const{ return bUnLoadAll_; }
		void UnLoadAll();	// 全ての画像とソースボイスの破棄

	private:
		std::unordered_map<const char*, Texture*> umTexturePtr_;
		std::vector<const char*> vecUnLoadImageList_;
		Texture* pNullTexture_ = nullptr;

		std::unordered_map<const char*, Sound*> umSoundPtr_;
		std::unordered_map<const char*, unsigned int> umSoundSize_;
		std::unordered_map<const char*, unsigned int> umSoundIndex_;
		std::vector<const char*> vecUnLoadWaveList_;
		Sound* pNullSound_ = nullptr;

		std::unordered_map<const char*, ObjModel*> umObjModelPtr_;
		std::vector<const char*> vecUnLoadObjModelList_;
		ObjModel* pNullObjModel_ = nullptr;

		std::unordered_map<const char*, PmxModel*> umPmxModelPtr_;
		std::vector<const char*> vecUnLoadPmxModelList_;
		PmxModel* pNullPmxModel_ = nullptr;

		std::unordered_map<const char*, VmdMotion*> umVmdMotionPtr_;
		std::vector<const char*> vecUnLoadVmdMotionList_;
		VmdMotion* pNullVmdMotion_ = nullptr;

		bool bUnLoadAll_ = false;

		StorageManager();					// コンストラクタ
		~StorageManager(){ UnLoadAll(); }	// デストラクタ

		// コピーコンストラクタの禁止
		StorageManager(const StorageManager& src){}
		StorageManager& operator=(const StorageManager& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
