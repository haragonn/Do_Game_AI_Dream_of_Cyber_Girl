/*==============================================================================
	[SoundManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SOUNDMANAGER_H
#define INCLUDE_IDEA_SOUNDMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <XAudio2.h>


namespace idea{
	class Sound;

	//------------------------------------------------------------------------------
	// クラス名　：SoundManager
	// クラス概要：シングルトン
	// 　　　　　　XAudio2オブジェクトへのインターフェイスとマスターボイスの管理を行う
	//------------------------------------------------------------------------------
	class SoundManager{
	public:
		// 唯一のインスタンスを返す
		static SoundManager& Instance()
		{
			static SoundManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init();
		
		// 終了処理
		void UnInit();

		// ソースボイスの生成
		bool CreateSourceVoice(IXAudio2SourceVoice** ppSourceVoice, WAVEFORMATEX* pSourceFormat);

		// サウンドの登録
		void Register(Sound* pSound);

		// サウンドの登録解除
		void UnRegister(Sound* pSound);

	private:
		// XAudio2オブジェクトへのインターフェイス
		IXAudio2* pXAudio2_ = nullptr;

		// マスターボイス
		IXAudio2MasteringVoice* pMasteringVoice_ = nullptr;

		// CoInitializeEx()したフラグ
		bool bCom_ = false;

		// リストの始まり
		Sound* pListBegin_ = nullptr;
		
		// リストの終わり
		Sound* pListEnd_ = nullptr;

		SoundManager();
		~SoundManager(){ UnInit(); }

		// コピーコンストラクタの禁止
		SoundManager(const SoundManager& src){}
		SoundManager& operator=(const SoundManager& src){}
	};
}

#endif // #ifndef INCLUDE_IDEA_SOUNDMANAGER_H