#include "Tutorial.h"
#include "MainGame.h"

using namespace idea;

// Tutorialの実装
class Tutorial::Impl{
public:
	// コントローラー入力
	Controller controller_;

	// 背景
	Sprite spriteBG_;

	// 背景のテクスチャー
	Texture textureBG_;

	// 操作説明
	Sprite spriteConfig_;

	// 操作説明のテクスチャー
	Texture textureConfig_;

	// キー入力を促すフォントの背景
	Sprite spritePressKey_;

	// キー入力を促すフォント
	Font fontPressKey_;

	// 操作説明フォント
	Font fontInfo_;

	// 決定音
	Sound seDecision_;

	// 背景のスクロール
	float scrollBG_ = 0.0f;

	// 終了フラグ
	bool isEnd_ = false;
};

Tutorial::Tutorial() :
	pImpl_(new Tutorial::Impl)
{
	Assert(pImpl_);
}

Tutorial::~Tutorial()
{
	SafeDelete(pImpl_);
}

void Tutorial::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::RETURN);
	impl.controller_.SetConfig(PadButton::START, KeyCode::SPACE);
	impl.controller_.Update();

	impl.spriteBG_.Init(C_W, C_H, S_W, S_W);

	impl.textureBG_.LoadImageFromStorage("asset/TEXTURE/grid00.bmp");

	impl.spriteConfig_.Init(C_W, C_H * 0.85f, 1000, 600);

	impl.textureConfig_.LoadImageFromStorage("asset/TEXTURE/config.png");

	impl.scrollBG_ = 0.0f;

	impl.spritePressKey_.Init(C_W, C_H * 1.8f, S_W * 0.7f, S_H * 0.1f);
	impl.spritePressKey_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.8f));

	impl.seDecision_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.seDecision_.SetVolume(0.5f);

	impl.fontInfo_.Init(72, 1000);
	impl.fontInfo_.SetColor(ideaColor::SOFT_BLACK);

	impl.fontPressKey_.Init(50, 1000, FontReferencePoint::CENTER);

	impl.isEnd_ = false;
}

void Tutorial::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン遷移
	if(fade_.IsFadeOutCompleted()){
		gameManager_.SetNextScenePtr(new MainGame);
		return;
	}

	impl.controller_.Update();

	// シーン遷移に合わせBGMのボリュームを下げる
	if(impl.isEnd_){
		fade_.SetFade(40);

		return;
	}

	// ボタン押下でフェード
	if(!fade_.IsFading()){
		if(impl.controller_.IsKeyPress(KeyCode::RETURN)){
			impl.isEnd_ = true;

			fade_.SetFade(40);

			impl.seDecision_.Play();
		}

		for(int i = 0; i < PadButton::BUTTON_MAX; ++i){
			if(impl.controller_.GetButton(i) == 1){
				impl.isEnd_ = true;

				fade_.SetFade(40);

				impl.seDecision_.Play();

				break;
			}
		}
	}

	impl.scrollBG_ += 0.00075f;
}

void Tutorial::Draw()
{
	// 実装
	auto& impl = (*pImpl_);

	// 背景
	impl.spriteBG_.SetColor(ideaColor::WHITE);
	impl.spriteBG_.DrawDelimitedTexture(impl.textureBG_, impl.scrollBG_ * 2.0f, -impl.scrollBG_ * 1.5f, 5.0f, 5.0f);

	// 操作方法表示
	impl.spriteConfig_.DrawTexture(impl.textureConfig_);

	// 全体を少し暗くする
	impl.spriteBG_.SetColor(ideaColor::SOFT_BLACK.ReplaceAlpha(0.1f));
	impl.spriteBG_.DrawRect();

	// キー入力を促す
	impl.spritePressKey_.DrawRect();
	impl.fontPressKey_.DrawFormatText(C_W, C_H * 1.8f, "ボタンまたはENTERキーを押して開始 ");

	// 操作説明
	impl.fontInfo_.DrawFormatText(0.0f, 0.0f, "操作説明");
}
