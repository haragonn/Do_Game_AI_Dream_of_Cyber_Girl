#include "Title.h"
#include "Menu.h"
#include "../GameObject/TitleRenderer.h"
#include "../GameObject/TitleCharacter.h"
#include "../GameObject/TitleUI.h"

using namespace idea;

// Titleの実装
class Title::Impl{
public:
	// コントローラー入力
	Controller controller_;

	// 背景
	Sprite spriteBG_;

	// 背景のテクスチャー
	Texture textureBG_;

	// 背景のスクロール
	float scrollBG_ = 0.0f;

	// BGM
	Sound bgm_;

	// BGMのボリューム
	float bgmVolume_ = 0.0f;

	// 決定音
	Sound seDecision_;

	// 終了フラグ
	bool isEnd_ = false;
};

Title::Title() :
	pImpl_(new Title::Impl)
{
	Assert(pImpl_);
}

Title::~Title()
{
	SafeDelete(pImpl_);
}

void Title::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	AddGameObject<TitleRenderer>(RENDERER_LAYER);

	AddGameObject<TitleCharacter>(TITLE_CHARACTER_LAYER);

	AddGameObject<TitleUI>(TITLE_UI_LAYER);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::RETURN);
	impl.controller_.Update();

	impl.spriteBG_.Init(C_W, C_H, S_W, S_W);
	impl.spriteBG_.SetColor(ideaColor::SOFT_BLUE);

	impl.textureBG_.LoadImageFromFile("asset/TEXTURE/grid04.bmp");

	impl.scrollBG_ = 0.0f;

	impl.isEnd_ = false;

	impl.bgm_.LoadWaveFromStorage("2_23_AM_2.wav");
	impl.bgmVolume_ = 1.0f;
	impl.bgm_.SetVolume(impl.bgmVolume_);

	impl.seDecision_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.seDecision_.SetVolume(0.5f);

	impl.bgm_.Play(true);
}

void Title::Update()
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

	impl.scrollBG_ += 0.00075f;

	// ボタン押下でフェード
	if(!fade_.IsFading()){
		for(int i = 0; i < PadButton::BUTTON_MAX; ++i){
			if(impl.controller_.GetButton(i) == 1){
				impl.isEnd_ = true;

				fade_.SetFade(40);

				impl.seDecision_.Play();

				break;
			}
		}
	}

	Scene::Update();
}

void Title::Draw()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.spriteBG_.DrawDelimitedTexture(impl.textureBG_, impl.scrollBG_ * 2.0f, -impl.scrollBG_ * 1.5f, 5.0f, 5.0f);

	Scene::Draw();
}
