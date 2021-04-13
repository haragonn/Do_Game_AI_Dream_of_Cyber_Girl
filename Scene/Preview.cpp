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

// Previewの実装
class Preview::Impl{
public:
	// コントローラー入力
	Controller controller_;

	// 説明書きフォント
	Font fontInfo_;

	// BGM
	Sound bgm_;

	// BGMのボリューム
	float bgmVolume_ = 0.0f;

	// 終了音
	Sound seEnd_;

	// 終了フラグ
	bool isEnd_ = false;

	// デバッグログ
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

	// 実装
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
	// 実装
	auto& impl = (*pImpl_);

	Scene::UnInit();

	impl.debugLog_.ClearLog();
}

void Preview::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン遷移
	if(fade_.IsFadeOutCompleted()){
		gameManager_.SetNextScenePtr(new Menu);

		return;
	}

	// シーン遷移に合わせBGMのボリュームを下げる
	if(impl.isEnd_){
		impl.bgmVolume_ -= 1.0f / 40.0f;
		impl.bgm_.SetVolume(impl.bgmVolume_);

		fade_.SetFade(40);

		return;
	}

	impl.controller_.Update();

	// ボタン押下でフェード
	if(!fade_.IsFading()
		&& impl.controller_.GetButton(PadButton::START) == 1){

		impl.isEnd_ = true;

		fade_.SetFade(40);

		impl.seEnd_.Play();
	}

	// vitaの追加
	if(impl.controller_.GetButton(PadButton::X) == 1){
		AddGameObject<Vita>(VITA_LAYER)->SetPosition(0.0f, 5.0f, 0.0f);
	}

	// vitaの削除
	if(impl.controller_.GetButton(PadButton::Y) == 1){
		auto vitas = GetGameObjects<Vita>(VITA_LAYER);

		if(vitas.size() > 1){
			vitas[1]->SetDestroy();
		}
	}

	Scene::Update();

	impl.debugLog_.ClearLog();

	// vitaの状態をデバッグ情報に書き込む
	auto& vita = (*GetGameObject<Vita>(VITA_LAYER));

	switch(vita.GetState()){
		case VitaState::Idol:
			impl.debugLog_.Log("AI State　　　　：Idol");
			break;

		case VitaState::Chase:
			impl.debugLog_.Log("AI State　　　　：Chase");
			break;

		case VitaState::StopBy:
			impl.debugLog_.Log("AI State　　　　：StopBy");
			break;

		case VitaState::Escape:
			impl.debugLog_.Log("AI State　　　　：Escape");
			break;

		case VitaState::Count:
			impl.debugLog_.Log("AI State　　　　：Count");
			break;

		case VitaState::Disappearance:
			impl.debugLog_.Log("AI State　　　　：Disappearance");
			break;
	}
}

void Preview::Draw()
{
	Scene::Draw();

	// 実装
	auto& impl = (*pImpl_);

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 0, "Aボタン・Aキー：追いかけさせる");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 1, "Bボタン・Sキー：逃げさせる");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 2, "Xボタン・Qキー：AI追加(耐久テスト用)");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 3, "Yボタン・Wキー：AI削除(耐久テスト用)");

	impl.fontInfo_.DrawFormatText(0.0f, 26.0f * 4, "スタートボタン・ENTERキー：終了");

	// vitaの状態を書き込む
	auto& vita = (*GetGameObject<Vita>(VITA_LAYER));
	switch(vita.GetState()){
		case VitaState::Idol:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State：Idol");
			break;
		case VitaState::Chase:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State：Chase");
			break;
		case VitaState::StopBy:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State：StopBy");
			break;
		case VitaState::Escape:
			impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 0.0f, "AI State：Escape");
			break;
	}

	// vitaの数を書き込む
	auto vitas = GetGameObjects<Vita>(VITA_LAYER);
	impl.fontInfo_.DrawFormatText(S_W - 26.0f * 8, 26.0f * 1, "AIの数  ：%3d", (int)vitas.size());
}
