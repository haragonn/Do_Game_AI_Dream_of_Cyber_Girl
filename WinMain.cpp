/*==============================================================================
	[WinMain.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "WinMain.h"
#include "idea/h/Framework.h"
#include "Scene/Title.h"
#include "Scene/Menu.h"
#include "Scene/Tutorial.h"
#include "Scene/MainGame.h"
#include "Scene/EditMode.h"
#include "Scene/Preview.h"

#include <string>

// 起動時にデバッグモードで開く確認をするか
#define BOOT_OPTION_SELECT (TRUE)

// 2重起動の防止をするか
#define PROHIBITION_OF_MULTIPLE_LAUNCHES (TRUE)

using namespace idea;

namespace{
	// ゲームタイトル
	static const char* GAME_TITLE = "ゲームAIは電脳少女の夢を見るか？";

	// フレームレート
	static const unsigned int FRAME_RATE = 60U;

	// フレームスキップ数
	static const unsigned int FRAME_SKIP = 0U;

	// 簡易fps表示
	static const bool SHOW_FPS = true;

	// ウィンドウモード
	static const bool WINDOW_MODE = true;

	// ネットワークの使用
	static const bool USE_NETWORK = false;

	// デバッグ情報表示の使用
	static const bool USE_DEBUG = true;
}

// デモのシーン(ゲームで使用するリソースも同時にロードする)
class Demo : public AsyncLoadScene{
public:
	// 非同期読み込み
	void Load()override
	{
		// テクスチャー
		storage_.LoadImageFromFile("asset/TEXTURE/title.png");
		storage_.LoadImageFromFile("asset/TEXTURE/balloon.png");
		storage_.LoadImageFromFile("asset/TEXTURE/grid00.bmp");
		storage_.LoadImageFromFile("asset/TEXTURE/grid04.bmp");
		storage_.LoadImageFromFile("asset/TEXTURE/grid05.bmp");
		storage_.LoadImageFromFile("asset/TEXTURE/config.png");
		storage_.LoadImageFromFile("asset/TEXTURE/player.png");
		storage_.LoadImageFromFile("asset/TEXTURE/vita.png");
		storage_.LoadImageFromFile("asset/TEXTURE/bar02.bmp");
		storage_.LoadImageFromFile("asset/TEXTURE/item.png");
		storage_.LoadImageFromFile("asset/TEXTURE/explosion.png");

		// 効果音
		storage_.LoadWaveFromFile("asset/SOUND/batyuun100.wav");
		storage_.LoadWaveFromFile("asset/SOUND/pyuwaan000.wav");
		storage_.LoadWaveFromFile("asset/SOUND/on000.wav");
		storage_.LoadWaveFromFile("asset/SOUND/off000.wav");
		storage_.LoadWaveFromFile("asset/SOUND/po000.wav");

		// 音声
		storage_.LoadWaveFromFile("asset/SOUND/voice_main_geme.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_edit_game.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_edit_mode.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_exit.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_start.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_player_touch.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_vita_touch.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_vita_touch_first.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_add_vita.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_add_vita2.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_add_vita3.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_flock_vita.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_player_win.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_vita_win.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_main_game_finish.wav");
		storage_.LoadWaveFromFile("asset/SOUND/voice_edit_game_finish.wav");

		// 音楽
		storage_.LoadWaveFromArchiveFile("sound.dat", "2_23_AM_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "SUMMER_TRIANGLE_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "極東の羊、テレキャスターと踊る_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "アトリエと電脳世界_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "Cassette_Tape_Dream_2.wav");

		// モデルデータ
		storage_.LoadPmxMeshFromFile("asset/MODEL/Player/Player.pmx");
		storage_.LoadPmxMeshFromFile("asset/MODEL/Vita/Vita.pmx");

		// モーションデータ
		storage_.LoadVmdMotionFromFile("asset/MOTION/_待機.vmd");
		storage_.LoadVmdMotionFromFile("asset/MOTION/歩く.vmd");
	}

	void Init()override
	{
		time_ = 0;
		anime_ = 0;
		scale_ = 15.0f;

		spriteBG_.Init(S_W * 0.5f, S_H * 0.5f, S_W, S_H);
		spriteIcon_.Init(S_W * 0.5f, S_H * 0.4f, S_W * 0.3f * scale_, S_W * 0.3f * scale_);
		spriteFont_.Init(S_W * 0.5f, S_H * 0.8f, S_W * 0.55f, S_W * 0.15f);
		spriteFont_.SetColor(1.0f, 1.0f, 1.0f, 0.0f);
		spriteCircle_.Init(S_W - 100.0f, S_H - 70.0f, 50.0f, 50.0f);
		spriteCircle_.SetColor(0.0f, 0.0f, 0.0f, 1.0f);


		texIcon_.LoadImageFromStorage("logo00.bmp", 4, 1);
		texFont_.LoadImageFromStorage("logo01.bmp");
		se_[0].LoadWaveFromStorage("se_logo00.wav");
		se_[1].LoadWaveFromStorage("se_logo01.wav");
	}

	void UnInit()override
	{
		storage_.UnLoadImage("logo00.bmp");
		storage_.UnLoadImage("logo01.bmp");
		storage_.UnLoadWave("se_logo00.wav");
		storage_.UnLoadWave("se_logo01.wav");
	}

	void Update()override
	{
		if(fade_.IsFadeOutCompleted()){
			gameManager_.SetNextScenePtr(new Title);
			return;
		}

		if(scale_ > 1.0f){
			scale_ *= 0.85f;
		} else{
			scale_ = 1.0f;
			if(anime_ < 31){
				++anime_;
			}
			if(anime_ == 1){
				se_[0].Play();
			}
			if(anime_ == 30){
				se_[1].Play();
			}
		}

		spriteIcon_.SetSize(SCREEN_WIDTH * 0.3f * scale_, SCREEN_WIDTH * 0.3f * scale_);

		if(time_ > 180 && IsLoadCompleted()){
			fade_.SetFade(40);
		}

		++time_;
	}

	void Draw()override
	{
		spriteBG_.DrawRect();

		if(time_ <= 30){
			spriteIcon_.SetColor(1.0f, 1.0f, 1.0f, time_ / 30.0f);
		}
		spriteIcon_.DrawDividedTexture(texIcon_, anime_ < 20 ? 0 : (anime_ - 20) / 3, 0);

		if(time_ >= 40 && time_ <= 90){
			spriteFont_.SetColor(1.0f, 1.0f, 1.0f, (time_ - 40.0f) / 50.0f);
		}
		spriteFont_.DrawTexture(texFont_);

		if(time_ > 240){
			spriteCircle_.DrawCircle((time_ - 240) / 120.0f);
		}
	}

private:
	// 経過時間
	int time_ = 0;

	// アニメーション再生時間
	int anime_ = 0;

	// アイコンの大きさ
	float scale_ = 15.0f;

	// 背景
	Sprite spriteBG_;

	// アイコン
	Sprite spriteIcon_;

	// 文字
	Sprite spriteFont_;

	// 読み込みを表す円
	Sprite spriteCircle_;

	// アイコンのテクスチャー
	Texture texIcon_;

	// 文字のテクスチャー
	Texture texFont_;

	// 効果音
	Sound se_[2];
};

// デモのデータをロードするシーン
class DemoLoad : public AsyncLoadScene{
public:
	void Load()override
	{
		storage_.LoadImageFromArchiveFile("logo.dat", "logo00.bmp");
		storage_.LoadImageFromArchiveFile("logo.dat", "logo01.bmp");
		storage_.LoadWaveFromArchiveFile("logo.dat", "se_logo00.wav");
		storage_.LoadWaveFromArchiveFile("logo.dat", "se_logo01.wav");
	}

	void Init()override
	{
		spriteBG_.Init(C_W, C_H, S_W, S_H);
		spriteBG_.SetColor(ideaColor::BLACK);
	}

	void UnInit()override{}

	void Update()override
	{
		if(fade_.IsFadeOutCompleted()){
			gameManager_.SetNextScenePtr(new Demo);
		}

		if(IsLoadCompleted()){
			fade_.SetFade(10, ideaColor::BLACK);
		}
	}

	void Draw()override
	{
		spriteBG_.DrawRect();
	}

private:
	// 背景
	Sprite spriteBG_;
};

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	// メモリリークのチェック
	SetLeakCheckFlag();

	// フレームワーク初期化情報
	FrameworkInitDesc fid = {};
	fid.width = SCREEN_WIDTH;
	fid.height = SCREEN_HEIGHT;
	fid.bWindowed = WINDOW_MODE;
	fid.fps = FRAME_RATE;
	fid.frameSkipMax = FRAME_SKIP;
	fid.bNetwork = USE_NETWORK;
	fid.bDebug = USE_DEBUG;
	fid.pClassName = GAME_TITLE;

	// デバッグモードで開く確認をするか
#if BOOT_OPTION_SELECT
	int isDebug = 0;
	isDebug = MessageBox(NULL, TEXT("デバッグモードで起動しますか？"), TEXT("起動オプション"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

	IniReader ir("debug.ini");

	if(isDebug == IDYES){
		ir.WriteInt("IMGUI", "SHOW", 1);
		fid.bDebug = true;
	} else{
		ir.WriteInt("IMGUI", "SHOW", 0);
		fid.bDebug = false;
	}
#endif

	// 2重起動の防止
#if PROHIBITION_OF_MULTIPLE_LAUNCHES
	HANDLE hMutex = CreateMutex(NULL, TRUE, GAME_TITLE);

	if(GetLastError() == ERROR_ALREADY_EXISTS){
		std::string mes = GAME_TITLE;
		mes += "は既に起動しています。";
		MessageBox(NULL, mes.c_str(), NULL, NULL);
		return -1;
	}
#endif

	// idea起動
	if(!Framework::Init(hInstance, nCmdShow, fid)){
		return -1;
	}

	// タイトルの表示
	Framework::SetWindowFormatText(GAME_TITLE);

	// フレームレートの簡易表示
	Framework::SetFpsSimpleDisplayFlag(SHOW_FPS);

	// マウスカーソルの表示設定
	Framework::SetMouseCursorVisibleFlag(true, true);

	Framework::Run(new DemoLoad);	// 実行
	//Framework::Run(new Title);	// 実行
	//Framework::Run(new Menu);		// 実行
	//Framework::Run(new Tutorial);	// 実行
	//Framework::Run(new MainGame);	// 実行
	//Framework::Run(new EditMode);	// 実行
	//Framework::Run(new Preview);	// 実行

	// 終了処理
	Framework::UnInit();

#if PROHIBITION_OF_MULTIPLE_LAUNCHES
	if(hMutex){
		// ミューテックスの解放
		CloseHandle(hMutex);
	}
#endif

	return 0;
}