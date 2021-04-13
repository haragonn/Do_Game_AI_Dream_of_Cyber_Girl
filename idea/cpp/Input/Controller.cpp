/*==============================================================================
	[Controller.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Network/NetworkManager.h"
#include "../../h/Input/InputManager.h"
#include "../../h/Input/Controller.h"

using namespace std;
using namespace idea;

struct PacketData NullInputData;

//------------------------------------------------------------------------------
// コンストラクタ
//------------------------------------------------------------------------------
Controller::Controller() :
	joystickID_(0U),
	bKeyboard_(false),
	up_(0U),
	down_(0U),
	left_(0U),
	right_(0U),
	lAxisX_(0.0f),
	lAxisY_(0.0f),
	rAxisX_(0.0f),
	rAxisY_(0.0f),
	lTrigger_(0.0f),
	rTrigger_(0.0f),
	bVibration_(false),
	vibrationTime_(0U),
	rMode_(ReplayMode::REPLAY_NONE)
{
	for(int i = BUTTON_MAX - 1; i >= 0; --i){
		button_[i] = 0U;
		config_[i] = i;
		keyConfig_[i] = KeyCode::KEY_CODE_UNUSED;
	}
}

//------------------------------------------------------------------------------
// 初期化
//------------------------------------------------------------------------------
void Controller::Init(unsigned int JoystickID, bool bKeyboard, bool bVibration)
{
	// ゼロクリア
	up_ = 0U;
	down_ = 0U;
	left_ = 0U;
	right_ = 0U;

	lAxisX_ = 0.0f;
	lAxisY_ = 0.0f;

	rAxisX_ = 0.0f;
	rAxisY_ = 0.0f;

	lTrigger_ = 0.0f;
	rTrigger_ = 0.0f;

	for(int i = BUTTON_MAX - 1; i >= 0; --i){
		button_[i] = 0U;
		config_[i] = (unsigned int)i;
		keyConfig_[i] = KeyCode::KEY_CODE_UNUSED;
	}

	vibrationTime_ = 0U;

	// リプレイデータの初期化
	std::vector<struct InputLog>().swap(replayLog_);
	itReplayLog_ = replayLog_.begin();

	bVibration_ = bVibration;
	joystickID_ = (JoystickID <= InputManager::JOYSTIC_MAX) ? JoystickID : 0U;
	bKeyboard_ = bKeyboard;
	rMode_ = ReplayMode::REPLAY_STOP;

	InputManager::Instance().SetVibration(joystickID_, 0.0f, 0.0f);
}

//------------------------------------------------------------------------------
// 終了処理
//------------------------------------------------------------------------------
void Controller::UnInit()
{
	InputManager::Instance().SetVibration(joystickID_, 0.0f, 0.0f);
	// 使用フラグをオフに
	joystickID_ = 0U;
	bKeyboard_ = false;
	rMode_ = ReplayMode::REPLAY_STOP;
}

//------------------------------------------------------------------------------
// 刺さっているジョイスティックの数の取得
//------------------------------------------------------------------------------
int Controller::GetJoystickMax()const
{
	return InputManager::Instance().GetJoystickNum();	// InputManagerの仲介をする
}

//------------------------------------------------------------------------------
// 入力のクリア
//------------------------------------------------------------------------------
void Controller::ZeroClear()
{
	// ゼロクリア
	up_ = 0U;
	down_ = 0U;
	left_ = 0U;
	right_ = 0U;
	lAxisX_ = 0.0f;
	lAxisY_ = 0.0f;
	rAxisX_ = 0.0f;
	rAxisY_ = 0.0f;
	lTrigger_ = 0.0f;
	rTrigger_ = 0.0f;
	for(int i = BUTTON_MAX - 1; i >= 0; --i){
		button_[i] = 0U;
	}
	vibrationTime_ = 0U;
}

//------------------------------------------------------------------------------
// 矢印キーの入力方向の取得(正規化済み)
//------------------------------------------------------------------------------
Vector2D Controller::GetArrow()
{
	Vector2D v;

	v.x = (((right_) ? 1.0f : 0.0f) + ((left_) ? -1.0f : 0.0f));
	v.y = (((up_) ? 1.0f : 0.0f) + ((down_) ? -1.0f : 0.0f));

	return v.Normalized();
}

//------------------------------------------------------------------------------
// 入力情報の更新
//------------------------------------------------------------------------------
void Controller::Update()
{
	if(joystickID_ || bKeyboard_ || rMode_ == ReplayMode::REPLAY_PLAY){	// 使用フラグがオフなら何もしない
		if(rMode_ != ReplayMode::REPLAY_PLAY){
			InputManager& im = InputManager::Instance();

			const Joystick& js = im.GetJoystick((DWORD)(joystickID_ - 1));

			bool bUp = (js.up || (bKeyboard_ && im.IsKeyPress(DIK_UP)));
			bool bDown = (js.down || (bKeyboard_ && im.IsKeyPress(DIK_DOWN)));
			bool bLeft = (js.left || (bKeyboard_ && im.IsKeyPress(DIK_LEFT)));
			bool bRight = (js.right || (bKeyboard_ && im.IsKeyPress(DIK_RIGHT)));

			bool bButton[BUTTON_MAX] = {};
			for(int i = BUTTON_MAX - 1; i >= 0; --i){
				bButton[i] = (js.button[config_[i]] || (bKeyboard_ && IsKeyPress(keyConfig_[i])));
			}

			++up_;
			if(!bUp){ up_ = 0U; }
			++down_;
			if(!bDown){ down_ = 0U; }
			++left_;
			if(!bLeft){ left_ = 0U; }
			++right_;
			if(!bRight){ right_ = 0U; }

			for(int i = BUTTON_MAX - 1; i >= 0; --i){
				++button_[i];
				if(!bButton[i]){ button_[i] = 0U; }
			}

			lAxisX_ = js.lAxisX;
			lAxisY_ = js.lAxisY;

			rAxisX_ = js.rAxisX;
			rAxisY_ = js.rAxisY;

			lTrigger_ = js.lTrigger;
			rTrigger_ = js.rTrigger;

			// リプレイデータの保存
			if(rMode_ == ReplayMode::REPLAY_REC){
				struct InputLog id = {};
				if(bUp)		{ id.keyData |= 0x8000; }
				if(bDown)	{ id.keyData |= 0x4000; }
				if(bLeft)	{ id.keyData |= 0x2000; }
				if(bRight)	{ id.keyData |= 0x1000; }

				if(bButton[0]){ id.keyData |= 0x800; }
				if(bButton[1]){ id.keyData |= 0x400; }
				if(bButton[2]){ id.keyData |= 0x200; }
				if(bButton[3]){ id.keyData |= 0x100; }
				if(bButton[4]){ id.keyData |= 0x80; }
				if(bButton[5]){ id.keyData |= 0x40; }
				if(bButton[6]){ id.keyData |= 0x20; }
				if(bButton[7]){ id.keyData |= 0x10; }
				if(bButton[8]){ id.keyData |= 0x8; }
				if(bButton[9]){ id.keyData |= 0x4; }

				id.lAxisX = (short)(lAxisX_ * 32767);
				id.lAxisY = (short)(lAxisY_ * 32767);

				id.rAxisX = (short)(rAxisX_ * 32767);
				id.rAxisY = (short)(rAxisY_ * 32767);

				id.lTrigger = (short)(lTrigger_ * 32767);
				id.rTrigger = (short)(rTrigger_ * 32767);

				replayLog_.push_back(id);
			}
		} else if(rMode_ == ReplayMode::REPLAY_PLAY){
			if(itReplayLog_ != replayLog_.end()){
				++up_;
				if(!(itReplayLog_->keyData & 0x8000)){ up_ = 0U; }
				++down_;
				if(!(itReplayLog_->keyData & 0x4000)){ down_ = 0U; }
				++left_;
				if(!(itReplayLog_->keyData & 0x2000)){ left_ = 0U; }
				++right_;
				if(!(itReplayLog_->keyData & 0x1000)){ right_ = 0U; }

				++button_[0];
				if(!(itReplayLog_->keyData & 0x800)){ button_[0] = 0U; }
				++button_[1];
				if(!(itReplayLog_->keyData & 0x400)){ button_[1] = 0U; }
				++button_[2];
				if(!(itReplayLog_->keyData & 0x200)){ button_[2] = 0U; }
				++button_[3];
				if(!(itReplayLog_->keyData & 0x100)){ button_[3] = 0U; }
				++button_[4];
				if(!(itReplayLog_->keyData & 0x80)){ button_[4] = 0U; }
				++button_[5];
				if(!(itReplayLog_->keyData & 0x40)){ button_[5] = 0U; }
				++button_[6];
				if(!(itReplayLog_->keyData & 0x20)){ button_[6] = 0U; }
				++button_[7];
				if(!(itReplayLog_->keyData & 0x10)){ button_[7] = 0U; }
				++button_[8];
				if(!(itReplayLog_->keyData & 0x8)){ button_[8] = 0U; }
				++button_[9];
				if(!(itReplayLog_->keyData & 0x4)){ button_[9] = 0U; }


				lAxisX_ = itReplayLog_->lAxisX / 32767.0f;
				lAxisY_ = itReplayLog_->lAxisY / 32767.0f;

				rAxisX_ = itReplayLog_->rAxisX / 32767.0f;
				rAxisY_ = itReplayLog_->rAxisY / 32767.0f;

				lTrigger_ = itReplayLog_->lTrigger / 32767.0f;
				rTrigger_ = itReplayLog_->rTrigger / 32767.0f;

				++itReplayLog_;
			} else{
				rMode_ = ReplayMode::REPLAY_STOP;

				up_ = 0U;
				down_ = 0U;
				left_ = 0U;
				right_ = 0U;

				for(int i = BUTTON_MAX - 1; i >= 0; --i){ button_[i] = 0U; }

				lAxisX_ = 0.0f;
				lAxisY_ = 0.0f;
				rAxisX_ = 0.0f;
				rAxisY_ = 0.0f;

				lTrigger_ = 0.0f;
				rTrigger_ = 0.0f;
			}
		}
	} else{
		up_ = 0U;
		down_ = 0U;
		left_ = 0U;
		right_ = 0U;

		for(int i = BUTTON_MAX - 1; i >= 0; --i){ button_[i] = 0U; }

		lAxisX_ = 0.0f;
		lAxisY_ = 0.0f;
		rAxisX_ = 0.0f;
		rAxisY_ = 0.0f;

		lTrigger_ = 0.0f;
		rTrigger_ = 0.0f;
	}

	if(vibrationTime_){
		--vibrationTime_;
		if(!vibrationTime_){
			InputManager::Instance().SetVibration(joystickID_, 0.0f, 0.0f);
		}
	}
}

//------------------------------------------------------------------------------
// ボタンの入力フレーム数の取得
//------------------------------------------------------------------------------
unsigned int Controller::GetButton(unsigned int buttonID)const
{
	if(BUTTON_MAX <= buttonID){ return 0U; }	// 存在しないボタンは0を返す
	return button_[buttonID];
}

//------------------------------------------------------------------------------
// 指定したキーが押されているか
//------------------------------------------------------------------------------
bool Controller::IsKeyPress(enum KeyCode keyCode)const
{
	return InputManager::Instance().IsKeyPress((DWORD)keyCode);	// InputManagerの仲介をする
}

//------------------------------------------------------------------------------
// 振動の開始
//------------------------------------------------------------------------------
bool Controller::SetVibration(unsigned int time, float leftSpeed, float rightSpeed)
{
	if(bVibration_){
		vibrationTime_ = time;
		return InputManager::Instance().SetVibration(joystickID_, leftSpeed, rightSpeed);
	}

	return false;
}

//------------------------------------------------------------------------------
// 指定したジョイスティックボタンをボタン番号に関連付ける
//------------------------------------------------------------------------------
void Controller::SetConfig(unsigned int configID, unsigned int buttonID)
{
	if(BUTTON_MAX <= buttonID || BUTTON_MAX <= configID){ return; }	// 存在しないボタンを指定されたら終了

	config_[configID] = buttonID;	// コンフィグ情報の更新
}

//------------------------------------------------------------------------------
// 指定したキーをボタン番号に関連付ける
//------------------------------------------------------------------------------
void Controller::SetConfig(unsigned int configID, enum KeyCode keyCode)
{
	if(BUTTON_MAX <= configID || KeyCode::KEY_CODE_MAX <= keyCode || keyCode < KeyCode::KEY_CODE_UNUSED){ return; }	// 存在しないボタンを指定されたら終了

	keyConfig_[configID] = keyCode;	// コンフィグ情報の更新
}

//------------------------------------------------------------------------------
// ボタン番号に関連付けられたジョイスティックボタンの番号を取得する
//------------------------------------------------------------------------------
int Controller::GetConfig(unsigned int configID)const
{
	if(BUTTON_MAX <= configID){ return -1; }	// 存在しないボタンを指定されたら(-1)を返す

	return (int)config_[configID];	// コンフィグ情報を返す
}

//------------------------------------------------------------------------------
// リプレイデータ記録開始
//------------------------------------------------------------------------------
bool idea::Controller::RecReplayData()
{
	if(rMode_ != ReplayMode::REPLAY_STOP){ return false; }
	else{
		std::vector<struct InputLog>().swap(replayLog_);

		rMode_ = ReplayMode::REPLAY_REC;

		return true;
	}
}

//------------------------------------------------------------------------------
// リプレイデータ再生開始
//------------------------------------------------------------------------------
bool idea::Controller::PlayReplayData(bool bCueing)
{
	if(rMode_ == ReplayMode::REPLAY_PLAY){ return false; }
	else{
		rMode_ = ReplayMode::REPLAY_PLAY;

		if(bCueing){
			itReplayLog_ = replayLog_.begin();
		}

		return true;
	}
}

//------------------------------------------------------------------------------
// リプレイデータの停止
//------------------------------------------------------------------------------
bool idea::Controller::StopReplayData()
{
	if(rMode_ == ReplayMode::REPLAY_STOP){ return false; }
	else{
		rMode_ = ReplayMode::REPLAY_STOP;

		return true;
	}
}

//------------------------------------------------------------------------------
// リプレイデータの消去
//------------------------------------------------------------------------------
bool idea::Controller::ClearReplayData()
{
	if(rMode_ == ReplayMode::REPLAY_REC || rMode_ == ReplayMode::REPLAY_PLAY){ return false; }
	else{
		std::vector<struct InputLog>().swap(replayLog_);

		rMode_ = ReplayMode::REPLAY_STOP;

		return true;
	}
}

//------------------------------------------------------------------------------
// リプレイデータの保存
//------------------------------------------------------------------------------
bool Controller::SaveReplayData(const char* pFileName)
{
	if(replayLog_.size() == 0){ return false; }

	FILE* fp = nullptr;
	if(fopen_s(&fp, pFileName, "wb") != 0){ return false; }

	std::vector<struct InputLog>::iterator it = replayLog_.begin(), itEnd = replayLog_.end();
	for(; it != itEnd; ++it){
		if(fwrite(&*it, sizeof(struct InputLog), 1, fp) == 0){
			fclose(fp);
			return false;
		}
	}

	fclose(fp);

	return true;
}

//------------------------------------------------------------------------------
// リプレイデータの読み込み
//------------------------------------------------------------------------------
bool Controller::LoadReplayData(const char* pFileName)
{
	FILE* fp = nullptr;
	if(fopen_s(&fp, pFileName, "rb") != 0){ return false; }

	std::vector<struct InputLog>().swap(replayLog_);

	while(!feof(fp)){
		struct InputLog buf = {};
		if((fread(&buf, 1, sizeof(buf), fp)) == 0){ break; }
		replayLog_.push_back(buf);
	}

	fclose(fp);

	itReplayLog_ = replayLog_.begin();

	return true;
}

//------------------------------------------------------------------------------
// リプレイが録画中か
//------------------------------------------------------------------------------
bool idea::Controller::IsRecordingReplayData() const
{
	return (rMode_ == ReplayMode::REPLAY_REC);
}

//------------------------------------------------------------------------------
// リプレイが再生中か
//------------------------------------------------------------------------------
bool Controller::IsPlayingReplayData()const
{
	return (rMode_ == ReplayMode::REPLAY_PLAY);
}