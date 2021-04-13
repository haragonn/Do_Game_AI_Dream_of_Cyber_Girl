#ifndef INCLUDE_IDEA_STORAGE_H
#define INCLUDE_IDEA_STORAGE_H

class Storage{
public:
	// 画像をファイルから読み込む
	bool LoadImageFromFile(const char* pFileName);

	// 画像をアーカイブファイルから読み込む
	bool LoadImageFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// 画像の破棄
	void UnLoadImage(const char* pFileName);

	// waveをファイルから読み込む
	bool LoadWaveFromFile(const char* pFileName, unsigned int soundSize = 1U);

	// waveをアーカイブファイルから読み込む
	bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName, unsigned int soundSize = 1U);

	// waveの破棄
	void UnLoadWave(const char* pFileName);

	// objメッシュをファイルから読み込む
	bool LoadObjMeshFromFile(const char* pFileName);

	// objメッシュをアーカイブファイルから読み込む
	bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// objメッシュの破棄
	void UnLoadObjMesh(const char* pFileName);

	// pmxメッシュをファイルから読み込む
	bool LoadPmxMeshFromFile(const char* pFileName);

	// pmxメッシュをアーカイブファイルから読み込む
	bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// pmxメッシュの破棄
	void UnLoadPmxMesh(const char* pFileName);

	// vmdモーションをファイルから読み込む
	bool LoadVmdMotionFromFile(const char* pFileName);

	// vmdモーションをアーカイブファイルから読み込む
	bool LoadVmdMotionFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

	// vmdモーションの破棄
	void UnLoadVmdMotion(const char* pFileName);

	// 全てのリソースの破棄
	void UnLoadAll();
};

#endif	// #ifndef INCLUDE_IDEA_STORAGE_H