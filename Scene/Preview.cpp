#include "Preview.h"
#include "Menu.h"
#include "../GameObject/Renderer.h"
#include "../GameObject/Player.h"
#include "../GameObject/Vita.h"
#include "../GameObject/Map.h"
#include "../GameObject/MetaAI.h"
#include "../GameObject/Judg.h"
#include "../GameObject/Collider.h"

using namespace std;
using namespace idea;

// Preview�̎���
class Preview::Impl{
public:
	// �R���g���[���[����
	Controller controller_;

	// ���������t�H���g
	Font fontInfo_;

	// BGM
	Sound bgm_;

	// BGM�̃{�����[��
	float bgmVolume_ = 0.0f;

	// �I����
	Sound seEnd_;

	// �I���t���O
	bool isEnd_ = false;

	// �f�o�b�O���O
	Debug debugLog_;
};

Preview::Preview()
	: pImpl_(new Preview::Impl)
{
	Assert(pImpl_);
}
Preview::~Preview()
{
	SafeDelete(pImpl_);
}

void Preview::Init()
{
	AddGameObject<Renderer>(RENDERER_LAYER);

	AddGameObject<Map>(MAP_LAYER);

	AddGameObject<MetaAI>(META_AI_LAYER);

	AddGameObject<Judg>(JUDG_LAYER);

	AddGameObject<Collider>(COLLIDER_LAYER);

	// ����
	auto& impl = (*pImpl_);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::X, KeyCode::Q);
	impl.controller_.SetConfig(PadButton::Y, KeyCode::W);
	impl.controller_.SetConfig(PadButton::START, KeyCode::RETURN);
	impl.controller_.Update();

	impl.fontInfo_.Init(26);

	impl.isEnd_ = false;

	impl.bgm_.LoadWaveFromStorage("Cassette_Tape_Dream_2.wav");
	impl.bgmVolume_ = 1.0f;
	impl.bgm_.SetVolume(impl.bgmVolume_);

	impl.seEnd_.LoadWaveFromStorage("asset/SOUND/off000.wav");
	impl.seEnd_.SetVolume(0.5f);

	impl.bgm_.Play(true);
}

void Preview::UnInit()
{
	// ����
	auto& impl = (*pImpl_);

	Scene::UnInit();

	impl.debugLog_.ClearLog();
}

void Preview::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[���J��
	if(fade_.IsFadeOutCompleted()){
		gameManager_.SetNextScenePtr(new Menu);

		return;
	}

	// �V�[���J�ڂɍ��킹BGM�̃{�����[����������
	if(impl.isEnd_){
		impl.bgmVolume_ -= 1.0f / 40.0f;
		impl.bgm_.SetVolume(impl.bgmVolume_);

		fade_.SetFade(40);

		return;
	}

	impl.controller_.Update();

	// �{�^�������Ńt�F�[�h
	if(!fade_.IsFading()
		&& impl.controller_.GetButton(PadButton::START) == 1){

		impl.isEnd_ = true;

		fade_.SetFade(40);

		impl.seEnd_.Play();
	}

	// vita�̒ǉ�
	if(impl.controller_.GetButton(PadButton::X) == 1){
		AddGameObject<Vita>(VITA_LAYER)->SetPosition(0.0f, 5.0f, 0.0f);
	}

	// vita�̍폜
	if(impl.controller_.GetButton(PadButton::Y) == 1){
		auto vitas = GetGameObjects<Vita>(VITA_LAYER);

		if(vitas.size() > 1){
			vitas[1]->SetDestroy();
		}
	}

	Scene::Update();

	impl.debugLog_.ClearLog();

	// vita�̏�Ԃ��f�o�b�O���ɏ�������
	auto& vita = (*GetGameObject<Vita>(VITA_LAYER));

	switch(vita.GetState()){
		case VitaState::Idol:
			impl.debugLog_.Log("AI State�@�@�@�@�FIdol");
			break;

		case VitaState::Chase:
			impl.debugLog_.Log("AI State�@�@�@�@�FChase");
			break;

		case VitaState::StopBy:
			impl.debugLog_.Log("AI State�@�@�@�@�FStopBy");
			break;

		case VitaState::Escape:
			impl.debugLog_.Log("AI State�@�@�@�@�FEscape");
			break;

		case VitaState::Count:
			impl.debugLog_.Log("AI State�@�@�@�@�FCount");
			break;

		case VitaState::Disappearance:
			impl.debugLog_.Log("AI State�@�@�@�@�FDisappearance");
			break;
	}
}

void Preview::Draw()
{
	Scene::Draw();

	// ����
	auto& impl = (*pImpl_);

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 0, "A�{�^���EA�L�[�F�ǂ�����������");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 1, "B�{�^���ES�L�[�F����������");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 2, "X�{�^���EQ�L�[�FAI�ǉ�(�ϋv�e�X�g�p)");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 3, "Y�{�^���EW�L�[�FAI�폜(�ϋv�e�X�g�p)");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 4, "�X�^�[�g�{�^���EENTER�L�[�F�I��");

	// vita�̏�Ԃ���������
	auto& vita = (*GetGameObject<Vita>(VITA_LAYER));
	switch(vita.GetState()){
		case VitaState::Idol:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State�FIdol");
			break;
		case VitaState::Chase:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State�FChase");
			break;
		case VitaState::StopBy:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State�FStopBy");
			break;
		case VitaState::Escape:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State�FEscape");
			break;
	}

	// vita�̐�����������
	auto vitas = GetGameObjects<Vita>(VITA_LAYER);
	impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 26.0f * 1, "AI�̐�  �F%3d", (int)vitas.size());
}
