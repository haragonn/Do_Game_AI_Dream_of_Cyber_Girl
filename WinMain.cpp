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

// �N�����Ƀf�o�b�O���[�h�ŊJ���m�F�����邩
#define BOOT_OPTION_SELECT (TRUE)

// 2�d�N���̖h�~�����邩
#define PROHIBITION_OF_MULTIPLE_LAUNCHES (TRUE)

using namespace idea;

namespace{
	// �Q�[���^�C�g��
	static const char* GAME_TITLE = "�Q�[��AI�͓d�]�����̖������邩�H";

	// �t���[�����[�g
	static const unsigned int FRAME_RATE = 60U;

	// �t���[���X�L�b�v��
	static const unsigned int FRAME_SKIP = 0U;

	// �Ȉ�fps�\��
	static const bool SHOW_FPS = true;

	// �E�B���h�E���[�h
	static const bool WINDOW_MODE = true;

	// �l�b�g���[�N�̎g�p
	static const bool USE_NETWORK = false;

	// �f�o�b�O���\���̎g�p
	static const bool USE_DEBUG = true;
}

// �f���̃V�[��(�Q�[���Ŏg�p���郊�\�[�X�������Ƀ��[�h����)
class Demo : public AsyncLoadScene{
public:
	// �񓯊��ǂݍ���
	void Load()override
	{
		// �e�N�X�`���[
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

		// ���ʉ�
		storage_.LoadWaveFromFile("asset/SOUND/batyuun100.wav");
		storage_.LoadWaveFromFile("asset/SOUND/pyuwaan000.wav");
		storage_.LoadWaveFromFile("asset/SOUND/on000.wav");
		storage_.LoadWaveFromFile("asset/SOUND/off000.wav");
		storage_.LoadWaveFromFile("asset/SOUND/po000.wav");

		// ����
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

		// ���y
		storage_.LoadWaveFromArchiveFile("sound.dat", "2_23_AM_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "SUMMER_TRIANGLE_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "�ɓ��̗r�A�e���L���X�^�[�Ɨx��_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "�A�g���G�Ɠd�]���E_2.wav");
		storage_.LoadWaveFromArchiveFile("sound.dat", "Cassette_Tape_Dream_2.wav");

		// ���f���f�[�^
		storage_.LoadPmxMeshFromFile("asset/MODEL/Player/Player.pmx");
		storage_.LoadPmxMeshFromFile("asset/MODEL/Vita/Vita.pmx");

		// ���[�V�����f�[�^
		storage_.LoadVmdMotionFromFile("asset/MOTION/_�ҋ@.vmd");
		storage_.LoadVmdMotionFromFile("asset/MOTION/����.vmd");
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
	// �o�ߎ���
	int time_ = 0;

	// �A�j���[�V�����Đ�����
	int anime_ = 0;

	// �A�C�R���̑傫��
	float scale_ = 15.0f;

	// �w�i
	Sprite spriteBG_;

	// �A�C�R��
	Sprite spriteIcon_;

	// ����
	Sprite spriteFont_;

	// �ǂݍ��݂�\���~
	Sprite spriteCircle_;

	// �A�C�R���̃e�N�X�`���[
	Texture texIcon_;

	// �����̃e�N�X�`���[
	Texture texFont_;

	// ���ʉ�
	Sound se_[2];
};

// �f���̃f�[�^�����[�h����V�[��
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
	// �w�i
	Sprite spriteBG_;
};

int APIENTRY WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_  LPTSTR lpCmdLine, _In_ int nCmdShow)
{
	// ���������[�N�̃`�F�b�N
	SetLeakCheckFlag();

	// �t���[�����[�N���������
	FrameworkInitDesc fid = {};
	fid.width = SCREEN_WIDTH;
	fid.height = SCREEN_HEIGHT;
	fid.bWindowed = WINDOW_MODE;
	fid.fps = FRAME_RATE;
	fid.frameSkipMax = FRAME_SKIP;
	fid.bNetwork = USE_NETWORK;
	fid.bDebug = USE_DEBUG;
	fid.pClassName = GAME_TITLE;

	// �f�o�b�O���[�h�ŊJ���m�F�����邩
#if BOOT_OPTION_SELECT
	int isDebug = 0;
	isDebug = MessageBox(NULL, TEXT("�f�o�b�O���[�h�ŋN�����܂����H"), TEXT("�N���I�v�V����"), MB_YESNO | MB_ICONQUESTION | MB_DEFBUTTON2);

	IniReader ir("debug.ini");

	if(isDebug == IDYES){
		ir.WriteInt("IMGUI", "SHOW", 1);
		fid.bDebug = true;
	} else{
		ir.WriteInt("IMGUI", "SHOW", 0);
		fid.bDebug = false;
	}
#endif

	// 2�d�N���̖h�~
#if PROHIBITION_OF_MULTIPLE_LAUNCHES
	HANDLE hMutex = CreateMutex(NULL, TRUE, GAME_TITLE);

	if(GetLastError() == ERROR_ALREADY_EXISTS){
		std::string mes = GAME_TITLE;
		mes += "�͊��ɋN�����Ă��܂��B";
		MessageBox(NULL, mes.c_str(), NULL, NULL);
		return -1;
	}
#endif

	// idea�N��
	if(!Framework::Init(hInstance, nCmdShow, fid)){
		return -1;
	}

	// �^�C�g���̕\��
	Framework::SetWindowFormatText(GAME_TITLE);

	// �t���[�����[�g�̊ȈՕ\��
	Framework::SetFpsSimpleDisplayFlag(SHOW_FPS);

	// �}�E�X�J�[�\���̕\���ݒ�
	Framework::SetMouseCursorVisibleFlag(true, true);

	Framework::Run(new DemoLoad);	// ���s
	//Framework::Run(new Title);	// ���s
	//Framework::Run(new Menu);		// ���s
	//Framework::Run(new Tutorial);	// ���s
	//Framework::Run(new MainGame);	// ���s
	//Framework::Run(new EditMode);	// ���s
	//Framework::Run(new Preview);	// ���s

	// �I������
	Framework::UnInit();

#if PROHIBITION_OF_MULTIPLE_LAUNCHES
	if(hMutex){
		// �~���[�e�b�N�X�̉��
		CloseHandle(hMutex);
	}
#endif

	return 0;
}