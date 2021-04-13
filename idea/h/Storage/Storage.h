#ifndef INCLUDE_IDEA_STORAGE_H
#define INCLUDE_IDEA_STORAGE_H

class Storage{
public:
	// �摜���t�@�C������ǂݍ���
	bool LoadImageFromFile(const char* pFileName);

	// �摜���A�[�J�C�u�t�@�C������ǂݍ���
	bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// �摜�̔j��
	void UnLoadImage(const char* pFileName);

	// wave���t�@�C������ǂݍ���
	bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);

	// wave���A�[�J�C�u�t�@�C������ǂݍ���
	bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);

	// wave�̔j��
	void UnLoadWave(const char* pFileName);

	// obj���b�V�����t�@�C������ǂݍ���
	bool LoadObjMeshFromFile(const char* pFileName);

	// obj���b�V�����A�[�J�C�u�t�@�C������ǂݍ���
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// obj���b�V���̔j��
	void UnLoadObjMesh(const char* pFileName);

	// pmx���b�V�����t�@�C������ǂݍ���
	bool LoadPmxMeshFromFile(const char* pFileName);

	// pmx���b�V�����A�[�J�C�u�t�@�C������ǂݍ���
	bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// pmx���b�V���̔j��
	void UnLoadPmxMesh(const char* pFileName);

	// vmd���[�V�������t�@�C������ǂݍ���
	bool LoadVmdMotionFromFile(const char* pFileName);

	// vmd���[�V�������A�[�J�C�u�t�@�C������ǂݍ���
	bool LoadVmdMotionFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// vmd���[�V�����̔j��
	void UnLoadVmdMotion(const char* pFileName);

	// �S�Ẵ��\�[�X�̔j��
	void UnLoadAll();
};

#endif	// #ifndef INCLUDE_IDEA_STORAGE_H