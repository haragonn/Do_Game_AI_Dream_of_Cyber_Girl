#include "GameUI.h"
#include "Player.h"
#include "Judg.h"
#include "Collider.h"

using namespace idea;

class GameUI::Impl{
public:
	// �I�j�\���p�X�v���C�g
	Sprite spriteOni_;

	// �v���C���[�̃e�N�X�`���[
	Texture texturePlayer_;

	// vita�̃e�N�X�`���[
	Texture textureVita_;

	// �X�^�~�i�\���g
	Sprite spriteStaminaFrame_;

	// �X�^�~�i�̋�̕���
	Sprite spriteStaminaBlank_;

	// �X�^�~�i�\���p�X�v���C�g
	Sprite spriteStamina_;

	// �p�����ԕ\���g
	Sprite spriteTimeFrame_;

	// �v���C���[�p�����Ԃ̋�̕���
	Sprite spritePlayerTimeBlank_;

	// �v���C���[�p�����ԗp�X�v���C�g
	Sprite spritePlayerTime_;

	// vita�p�����Ԃ̋�̕���
	Sprite spriteVitaTimeBlank_;

	// vita�p�����ԗp�X�v���C�g
	Sprite spriteVitaTime_;

	// �ėp�t�H���g
	Font font_;

	// �p�����ԕ\���p�t�H���g
	Font fontTime_[2];

	// �f�o�b�O���O
	Debug debugLog_;
};

GameUI::GameUI() :
	pImpl_(new GameUI::Impl)
{
	Assert(pImpl_);
}

GameUI::~GameUI()
{
	SafeDelete(pImpl_);
}

void GameUI::Init()
{
	// ����
	auto& impl = (*pImpl_);

	impl.spriteOni_.Init(S_H * 0.075f, S_H * 0.075f, S_H * 0.15f, S_H * 0.15f);

	impl.texturePlayer_.LoadImageFromStorage("asset/TEXTURE/player.png");
	impl.textureVita_.LoadImageFromStorage("asset/TEXTURE/vita.png");

	impl.spriteStaminaFrame_.Init(C_W, S_H * 0.9625f, S_W * 0.9f, S_H * 0.0575f);
	impl.spriteStaminaFrame_.SetColor(Color(0.2f, 0.3f, 0.475f, 1.0f));

	impl.spriteStaminaBlank_.Init(100.0f + 20.0f * 7.0f + S_W * 0.75f * 0.5f, S_H * 0.9625f, S_W * 0.75f, S_H * 0.025f);
	impl.spriteStaminaBlank_.SetColor(ideaColor::BLACK);

	impl.spriteStamina_.Init(100.0f + 20.0f * 7.0f + S_W * 0.75f * 0.5f, S_H * 0.9625f, S_W * 0.75f, S_H * 0.025f);
	impl.spriteStamina_.SetColor(ideaColor::ORANGE);

	impl.spriteTimeFrame_.Init(S_W * 0.775f, 35.0f, S_W * 0.4125f, 45.0f);
	impl.spriteTimeFrame_.SetColor(Color(0.2f, 0.3f, 0.475f, 1.0f));

	impl.spritePlayerTime_.Init(S_W * 0.775f, 25.0f, S_W * 0.4f, 15.0f);
	impl.spritePlayerTime_.SetColor(ideaColor::SOFT_RED.ReplaceGreen(0.3f).ReplaceBlue(0.4f) * 1.2f);

	impl.spritePlayerTimeBlank_.Init(S_W * 0.775f, 25.0f, S_W * 0.4f, 15.0f);
	impl.spritePlayerTimeBlank_.SetColor(ideaColor::BLACK);

	impl.spriteVitaTime_.Init(S_W * 0.775f, 45.0f, S_W * 0.4f, 15.0f);
	impl.spriteVitaTime_.SetColor(ideaColor::CYAN.ReplaceRed(0.5f));

	impl.spriteVitaTimeBlank_.Init(S_W * 0.775f, 45.0f, S_W * 0.4f, 15.0f);
	impl.spriteVitaTimeBlank_.SetColor(ideaColor::BLACK);

	impl.font_.Init(40);

	impl.fontTime_[0].Init(30, 500, FontReferencePoint::CENTER);
	impl.fontTime_[1].Init(24, 500, FontReferencePoint::CENTER);
}

void GameUI::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// �X�^�~�i�\���X�v���C�g���T�C�Y
	{
		// �X�^�~�i
		float stamina = player.GetStamina();

		// �X�^�~�i�\���o�[�̒��S
		float centerPosition = 100.0f + 20.0f * 7.0f + S_W * 0.75f * 0.5f * player.GetStamina() / 300.0f;

		// �X�^�~�i�\���o�[�̕�
		float whidth = S_W * 0.75f * player.GetStamina() / 300.0f;

		impl.spriteStamina_.Init(centerPosition, S_H * 0.9625f, whidth, S_H * 0.025f);

		// �\���F�ύX
		if(player.GetDoping()){
			impl.spriteStamina_.SetColor(ideaColor::SOFT_BLUE * 2.0f);
		} else if(player.GetExhausted()){
			impl.spriteStamina_.SetColor(ideaColor::SOFT_RED * 0.7f);
		} else{
			impl.spriteStamina_.SetColor(ideaColor::ORANGE);
		}
	}

	// �v���C���[�p�����ԕ\���X�v���C�g���T�C�Y
	{
		// �v���C���[�̌p������
		int playerTime = scene.GetGameObject<Judg>(JUDG_LAYER)->GetPlayerTime();

		// �p�����ԕ\���o�[�̒��S
		float centerPosition = S_W * 0.575f + S_W * 0.4f * 0.5f * playerTime / TIME_UP;

		// �p�����ԕ\���o�[�̕�
		float whidth = S_W * 0.4f * playerTime / TIME_UP;

		impl.spritePlayerTime_.Init(centerPosition, 25.0f, whidth, 15.0f);
		impl.spritePlayerTime_.SetColor(ideaColor::SOFT_RED.ReplaceGreen(0.3f).ReplaceBlue(0.4f));
	}

	// vita�p�����ԕ\���X�v���C�g���T�C�Y
	{
		// vita�̌p������
		int vitaTime = scene.GetGameObject<Judg>(JUDG_LAYER)->GetVitaTime();

		// �p�����ԕ\���o�[�̒��S
		float centerPosition = S_W * 0.575f + S_W * 0.4f * 0.5f * vitaTime / TIME_UP;

		// �p�����ԕ\���o�[�̕�
		float whidth = S_W * 0.4f * vitaTime / TIME_UP;

		impl.spriteVitaTime_.Init(centerPosition, 45.0f, whidth, 15.0f);
		impl.spriteVitaTime_.SetColor(ideaColor::CYAN.ReplaceRed(0.5f));
	}
}

void GameUI::Draw()
{
	// ����
	auto& impl = (*pImpl_);
	auto& scene = (*pScene_);

	bool isPlayerOni = scene.GetGameObject<Judg>(JUDG_LAYER)->IsPlayerOni();

	// �I�j�T�C�h�\��
	if(isPlayerOni){
		impl.spriteOni_.SetSize(S_H * 0.15f, S_H * 0.15f);
		impl.spriteOni_.SetColor(ideaColor::SOFT_RED.ReplaceGreen(0.3f).ReplaceBlue(0.4f));
		impl.spriteOni_.DrawRect();
		impl.spriteOni_.SetSize(S_H * 0.15f - 15.0f, S_H * 0.15f - 15.0f);
		impl.spriteOni_.SetColor(ideaColor::WHITE);
		impl.spriteOni_.DrawTexture(impl.texturePlayer_);

		impl.font_.DrawFormatText(S_H * 0.15f + 5.0f, 5.0f, "���Ȃ����I�j");
	} else{
		impl.spriteOni_.SetSize(S_H * 0.15f, S_H * 0.15f);
		impl.spriteOni_.SetColor(ideaColor::CYAN.ReplaceRed(0.5f));
		impl.spriteOni_.DrawRect();
		impl.spriteOni_.SetSize(S_H * 0.15f - 15.0f, S_H * 0.15f - 15.0f);
		impl.spriteOni_.SetColor(ideaColor::WHITE);
		impl.spriteOni_.DrawTexture(impl.textureVita_);

		impl.font_.DrawFormatText(S_H * 0.15f + 5.0f, 5.0f, "AI���I�j");
	}

	// �X�^�~�i�\��
	impl.spriteStaminaFrame_.DrawRect();
	impl.font_.DrawFormatText(80.0f, S_H - 45.0f, "STAMINA");
	impl.spriteStaminaBlank_.DrawRect();
	impl.spriteStamina_.DrawRect();

	// �p�����ԕ\��
	int playerTime = scene.GetGameObject<Judg>(JUDG_LAYER)->GetPlayerTime();
	int vitaTime = scene.GetGameObject<Judg>(JUDG_LAYER)->GetVitaTime();

	impl.fontTime_[0].DrawFormatText(C_W * 0.9f, 20.0f, "���Ȃ� %1d:%02d", FrameToMinute(playerTime), FrameToSecond(playerTime) % 60);
	impl.fontTime_[0].DrawFormatText(C_W * 0.9f, 50.0f, "  AI   %1d:%02d", FrameToMinute(vitaTime), FrameToSecond(vitaTime) % 60);

	impl.fontTime_[1].DrawFormatText(C_W * 0.9f, 22.0f, "                 %02d", (playerTime * 100 / 60) % 100);
	impl.fontTime_[1].DrawFormatText(C_W * 0.9f, 52.0f, "                 %02d", (vitaTime * 100 / 60) % 100);

	impl.spriteTimeFrame_.DrawRect();
	impl.spritePlayerTimeBlank_.DrawRect();
	impl.spritePlayerTime_.DrawRect();
	impl.spriteVitaTimeBlank_.DrawRect();
	impl.spriteVitaTime_.DrawRect();
}