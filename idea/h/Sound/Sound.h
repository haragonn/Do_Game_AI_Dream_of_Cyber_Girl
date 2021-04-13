/*==============================================================================
	[Sound.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLIDE_IDEA_SOUND_H
#define INCLIDE_IDEA_SOUND_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct IXAudio2SourceVoice;

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Sound
	// クラス概要：waveファイルの読み込み,ソースボイスの管理,再生を行う
	//------------------------------------------------------------------------------
	class Sound{
	public:
		Sound();
		~Sound();

		// waveファイルからサウンドを読み込む
		bool LoadWaveFromFile(const char* pFileName);

		// アーカイブファイルの中のwaveファイルからサウンドを読み込む
		bool LoadWaveFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// ストレージからサウンドを読み込む
		bool LoadWaveFromStorage(const char* pFileName);

		// ソースボイスの破棄
		void UnLoad();

		// 音楽の再生
		void Play(bool bLoop = false, bool bCueing = true, UINT32 begin = 0);

		// 音楽の停止
		void Stop();

		// 音楽の頭出し
		void Cueing();

		// 音量の設定
		void SetVolume(float volume = 1.0f);

		// 再生中か
		bool IsPlaying();

		// ループしているか
		bool IsLooping();

	private:
		friend class SoundManager;

		// ファイル名
		char fileName_[256] = {};

		// ソースボイス
		IXAudio2SourceVoice* pSourceVoice_ = nullptr;

		// オーディオ情報
		BYTE* pDataAudio_ = nullptr;

		// オーディオサイズ
		DWORD audioSize_ = 0U;

		// 再生中か
		bool bPlay_ = false;

		// ループしているか
		bool bLoop_ = false;

		// ストレージ使用フラグ
		bool bStorage_ = false;

		// 登録フラグ
		bool bRegistered_ = false;

		// 前のノード
		Sound* pPrev_ = nullptr;

		// 次のノード
		Sound* pNext_ = nullptr;

		// コピーコンストラクタの禁止
		Sound(const Sound& src){}
		Sound& operator=(const Sound& src){}
	};
}

#endif // #ifndef INCLIDE_IDEA_SOUND_H