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
	// �N���X���@�FMemoryManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@���\�[�X��ǂݍ���,�V�[�����܂����Ń��\�[�X��ێ�����N���X
	//------------------------------------------------------------------------------
	class StorageManager{
	public:
		// �B��̃C���X�^���X��Ԃ�
		static StorageManager& Instance()
		{
			static StorageManager s_Instance;
			return s_Instance;
		}

		void CreatNullInstance();

		void ReleaseNullInstance();

		// �摜�̓ǂݍ���
		bool LoadImageFromFile(const char* pFileName);
		
		// �A�[�J�C�u�t�@�C������摜��ǂݍ���
		bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);
		
		// �摜�̎擾
		Texture& GetTexture(const char* pFileName);
		
		// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩
		bool HasKeyTexture(const char* pFileName);

		// wave�t�@�C������T�E���h��ǂݍ���
		bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);
		
		// �A�[�J�C�u�t�@�C���̒���wave�t�@�C������T�E���h��ǂݍ���
		bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);

		// �T�E���h�̎擾
		Sound& GetSound(const char* pFileName);

		// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�T�E���h�����݂��邩
		bool HasKeyWave(const char* pFileName);

		void SetUnLoadImageRequest(const char* pFileName);
		inline int GetUnLoadImageRequestSize()const{ return vecUnLoadImageList_.size(); }
		void UnLoadImage();

		void SetUnLoadWaveRequest(const char* pFileName);
		inline int GetUnLoadWaveRequestSize()const{ return vecUnLoadWaveList_.size(); }
		void UnLoadWave();


		bool LoadObjMeshFromFile(const char* pFileName);	// �摜�̓ǂݍ���
		bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
		ObjModel& GetObjModel(const char* pFileName);	// �摜�̎擾
		bool HasKeyObjModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩

		bool LoadPmxMeshFromFile(const char* pFileName);	// �摜�̓ǂݍ���
		bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
		PmxModel& GetPmxModel(const char* pFileName);	// �摜�̎擾
		bool HasKeyPmxModel(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩

		bool LoadVmdMotionFromFile(const char* pFileName);	// �摜�̓ǂݍ���
		bool LoadVmdMotionFromArchiveFile(const char* pArchiveFileName, const char* pFileName);	// �A�[�J�C�u�t�@�C������摜��ǂݍ���
		VmdMotion& GetVmdMotion(const char* pFileName);	// �摜�̎擾
		bool HasKeyVmdMotion(const char* pFileName);	// �w�肵���t�@�C�����Ɗ֘A�t����ꂽ�摜�����݂��邩

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
		void UnLoadAll();	// �S�Ẳ摜�ƃ\�[�X�{�C�X�̔j��

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

		StorageManager();					// �R���X�g���N�^
		~StorageManager(){ UnLoadAll(); }	// �f�X�g���N�^

		// �R�s�[�R���X�g���N�^�̋֎~
		StorageManager(const StorageManager& src){}
		StorageManager& operator=(const StorageManager& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_STORAGEMANAGER_H
