#include "EditMode.h"
#include "Preview.h"
#include "../GameObject/EditRenderer.h"
#include "../GameObject/Panel.h"
#include "../GameObject/Block.h"
#include "../GameObject/Player.h"
#include "../GameObject/Vita.h"

using namespace std;
using namespace idea;

// マップ情報ID
namespace{
	// 床
	static const int FLOOR = 0;
	
	// 箱
	static const int BOX = 1;

	// 右上がりの坂
	static const int UPPER_RIGHT = 2;

	// 左上がりの坂
	static const int UPPER_LEFT = 3;

	// 奥上がりの坂
	static const int UPPER_BACK = 4;

	// 手前上がりの坂
	static const int UPPER_FRONT = 5;
	
	// プレイヤー
	static const int PLAYER = 6;
	
	// vita
	static const int VITA = 7;
};

// 設置モード
enum class SetMode{
	Box,	// 箱
	Slope,	// 坂
	Player,	// プレイヤー
	Vita	// AI
};

// EditModeの実装
class EditMode::Impl{
public:
	// コントローラー入力
	Controller controller_;

	// マウス入力
	Mouse mouse_;

	// 床
	PlaneMesh plane_;

	// 床のテクスチャー
	Texture texturePlane_;

	// マップの行数
	int row_ = 0;

	// マップの列数
	int col_ = 0;

	// マップデータ
	DynamicDualArray<int> mapData_;

	// 設置モード
	SetMode setMode_;

	// クリックフラグ
	bool isClick_ = false;

	// BGM
	Sound bgm_;

	// BGMのボリューム
	float bgmVolume_ = 0.0f;

	// 決定音
	Sound seDecision_;

	// 終了フラグ
	bool isEnd_ = false;

	// 操作法表示用フォント
	Font fontInfo_;

	// 設置モード表示用フォント
	Font fontMode_;
};

EditMode::EditMode()
	: pImpl_(new EditMode::Impl)
{
	Assert(pImpl_);
}

EditMode::~EditMode()
{
	SafeDelete(pImpl_);
}

void EditMode::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	// レンダラー
	auto& renderer = (*AddGameObject<EditRenderer>(EDIT_RENDERER_LAYER));

	impl.row_ = 11;
	impl.col_ = 11;

	impl.isClick_ = false;

	impl.controller_.Init(1, true);
	impl.controller_.SetConfig(PadButton::A, KeyCode::Z);
	impl.controller_.SetConfig(PadButton::START, KeyCode::RETURN);
	impl.controller_.Update();

	impl.texturePlane_.LoadImageFromStorage("asset/TEXTURE/grid05.bmp");

	impl.plane_.Init(0.0f, 0.0f, 0.0f);
	impl.plane_.Create(0.0f, 0.0f, 10.0f, 10.0f, impl.row_, impl.col_);
	impl.plane_.SetTexture(impl.texturePlane_);
	renderer.AddObject(impl.plane_);

	// ポイント判定用パネル設置
	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// パネル生成
			auto& panel = (*AddGameObject<Panel>(PANEL_LAYER));

			// ポジションの設定
			Vector3D pos;
			pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
			pos.y = 0.0f;
			pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);
			panel.SetPosition(pos);

			panel.SetCell(i, j);

			renderer.AddObject(panel.GetCube());
		}
	}

	// プレイヤー設置
	{
		// プレイヤー生成
		auto& player = (*AddGameObject<Player>(PLAYER_LAYER));

		// ポジションの設定
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, 5, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, 7, impl.row_);
		player.SetPosition(pos);

		player.SetCell(7, 5);
	}

	// vita設置
	{
		// ita生成
		auto& vita = (*AddGameObject<Vita>(VITA_LAYER));

		// ポジションの設定
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, 5, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, 3, impl.row_);

		vita.SetPosition(pos);

		vita.SetCell(3, 5);
	}

	impl.mapData_.Reserve(impl.col_, impl.row_, 0);

	impl.fontInfo_.Init(24);
	impl.fontMode_.Init(40);

	impl.setMode_ = SetMode::Box;

	impl.isEnd_ = false;

	impl.bgm_.LoadWaveFromStorage("アトリエと電脳世界_2.wav");
	impl.bgmVolume_ = 1.0f;
	impl.bgm_.SetVolume(impl.bgmVolume_);

	impl.seDecision_.LoadWaveFromStorage("asset/SOUND/on000.wav");
	impl.seDecision_.SetVolume(0.5f);

	impl.bgm_.Play(true);
}

void EditMode::UnInit()
{
	Scene::UnInit();
}

void EditMode::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	Scene::Update();

	// シーン遷移
	if(fade_.IsFadeOutCompleted()){
		WriteMapData();

		gameManager_.SetNextScenePtr(new Preview);

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

	// Enterキー押下でフェード
	if(!fade_.IsFading()
		&& impl.controller_.GetButton(PadButton::START) == 1){
		impl.isEnd_ = true;

		fade_.SetFade(40);

		impl.seDecision_.Play();
	}

	// ホイール回転でモードチェンジ
	if(impl.mouse_.GetWheel() < 0.0f){
			switch(impl.setMode_){
				case SetMode::Box:
					impl.setMode_ = SetMode::Slope;
					break;

				case SetMode::Slope:
					impl.setMode_ = SetMode::Player;
					break;

				case SetMode::Player:
					impl.setMode_ = SetMode::Vita;
					break;

				case SetMode::Vita:
					impl.setMode_ = SetMode::Box;
					break;
			}
	} else if(impl.mouse_.GetWheel() > 0.0f){
		switch(impl.setMode_){
			case SetMode::Box:
				impl.setMode_ = SetMode::Vita;
				break;

			case SetMode::Slope:
				impl.setMode_ = SetMode::Box;
				break;

			case SetMode::Player:
				impl.setMode_ = SetMode::Slope;
				break;

			case SetMode::Vita:
				impl.setMode_ = SetMode::Player;
				break;
		}

	}

	// カメラ
	auto& camera = (GetGameObject<EditRenderer>(EDIT_RENDERER_LAYER)->GetWrapCamera());

	RemoveBlock(camera);

	SetObject(camera);

	UpdateMapData();

	if(!impl.mouse_.GetButton(0)){
		impl.isClick_ = false;
	}
}

void EditMode::Draw()
{
	Scene::Draw();

	// 実装
	auto& impl = (*pImpl_);

	// 操作説明
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 0, "左クリック");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 1, "設置");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 3, "右クリック");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 4, "消去");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 6, "ホイール回転");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 7, "設置オブジェクト切替");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 9, "矢印キー");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 10, "移動");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 12, "ENTERキー");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 13, "終了");

	// 設置モード表示
	switch(impl.setMode_){
		case SetMode::Box:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "BOXセット");
			break;

		case SetMode::Slope:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "SLOPEセット");
			break;

		case SetMode::Player:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "PLAYERセット");
			break;

		case SetMode::Vita:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "AIセット");
			break;
	}
}

void EditMode::SetObject(idea::Camera& camera)
{
	// 実装
	auto& impl = (*pImpl_);

	// ブロック消去後連続して判定しない
	if(impl.isClick_){
		return;
	}

	// パネルリスト
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// パネルの通し番号
	int panelCount = 0;

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// マウスに重なっていなければ終了
			if(!impl.mouse_.CheckHitRayToSphere(camera, panels[panelCount]->GetCube())){
				++panelCount;

				continue;
			}

			switch(impl.setMode_){
				case SetMode::Box:
					SetBlock(i, j, panelCount);
					break;

				case SetMode::Slope:
					SetSlope(i, j, panelCount);
					break;

				case SetMode::Player:
					SetPlayer(i, j, panelCount);
					break;

				case SetMode::Vita:
					SetVita(i, j, panelCount);
					break;
			}

			++panelCount;
		}
	}
}

bool EditMode::SetBlock(int& i, int& j, int count)
{
	// 実装
	auto& impl = (*pImpl_);

	// 床以外には置けない
	if(impl.mapData_[j][i] != FLOOR){
		return false;
	}

	// 坂の下にブロックを作れないようにする
	{
		// 設置可能か
		bool canSet = true;

		for(int k = 0; k < 3; ++k){
			for(int l = 0; l < 3; ++l){
				if(k != 1 && l != 1){
					continue;
				}

				int x = j + k - 1;
				int z = i + l - 1;

				if(x < 0 || impl.col_ < x){
					continue;
				}

				if(z < 0 || impl.row_ < z){
					continue;
				}

				if(k == 1){
					if(l < 1){
						if(impl.mapData_[x][z] == UPPER_LEFT){
							canSet = false;
						}
					} else{
						if(impl.mapData_[x][z] == UPPER_RIGHT){
							canSet = false;
						}
					}
				} else{
					if(k < 1){
						if(impl.mapData_[x][z] == UPPER_BACK){
							canSet = false;
						}
					} else{
						if(impl.mapData_[x][z] == UPPER_FRONT){
							canSet = false;
						}
					}
				}
			}
		}

		// 2重ループを抜ける
		if(!canSet){
			i = impl.col_;
			j = impl.row_;

			return false;
		}
	}

	// パネルリスト
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// パネル光る
	panels[count]->SetColor(ideaColor::ORANGE);

	// 設置フラグ
	bool isSet = false;

	// クリックで箱設置
	if(impl.mouse_.GetButton(0)){
		// ポジションの設定
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

		auto& block = (*AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(pos);
		block.SetStyle(BlockStyle::Box);
		block.SetCell(j, i);

		isSet = true;
	}

	// 2重ループを抜ける
	i = impl.col_;
	j = impl.row_;

	return isSet;
}

bool EditMode::SetSlope(int& i, int& j, int count)
{
	// 実装
	auto& impl = (*pImpl_);

	// 床以外には置けない
	if(impl.mapData_[j][i] != FLOOR){
		return false;
	}

	// パネルリスト
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	for(int k = 0; k < 3; ++k){
		for(int l = 0; l < 3; ++l){
			if(k != 1 && l != 1){
				continue;
			}

			int x = j + k - 1;
			int z = i + l - 1;

			if(x < 0 || impl.col_ < x){
				continue;
			}

			if(z < 0 || impl.row_ < z){
				continue;
			}

			// 坂の下がブロックなら作れない
			if(!(j == 0 || j == impl.col_ - 1
			   || i == 0 || i == impl.row_ - 1)){
				if(k == 1){
					if(l < 1){
						if(impl.mapData_[j][i + 1] != FLOOR
							&& impl.mapData_[j][i + 1] != PLAYER
							&& impl.mapData_[j][i + 1] != VITA){
							continue;
						}
					} else{
						if(impl.mapData_[j][i - 1] != FLOOR
							&& impl.mapData_[j][i - 1] != PLAYER
							&& impl.mapData_[j][i - 1] != VITA){
							continue;
						}
					}
				} else{
					if(k < 1){
						if(impl.mapData_[j + 1][i] != FLOOR
							&& impl.mapData_[j + 1][i] != PLAYER
							&& impl.mapData_[j + 1][i] != VITA){
							continue;
						}
					} else{
						if(impl.mapData_[j - 1][i] != FLOOR
							&& impl.mapData_[j - 1][i] != PLAYER
							&& impl.mapData_[j - 1][i] != VITA){
							continue;
						}
					}
				}
			}

			// 箱に隣接時のみ設置可能
			if(impl.mapData_[x][z] == 1){
				// パネル光る
				panels[count]->SetColor(ideaColor::ORANGE);

				// 左ボタンが押されていれば坂を置く
				if(impl.mouse_.GetButton(0)){
					auto& block = (*AddGameObject<Block>(BLOCK_LAYER));

					// ポジションの設定
					Vector3D pos;
					pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
					pos.y = 0.0f;
					pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

					block.SetPosition(pos);

					// 相対位置から形を決定
					if(k == 1){
						if(l < 1){
							block.SetStyle(BlockStyle::UpperLeft);
						} else{
							block.SetStyle(BlockStyle::UpperRight);
						}
					} else{
						if(k < 1){
							block.SetStyle(BlockStyle::UpperBack);
						} else{
							block.SetStyle(BlockStyle::UpperFront);
						}
					}

					block.SetCell(j, i);

					// 2重ループを抜ける
					i = impl.col_;
					j = impl.row_;

					return true;
				}
			}
		}
	}

	return false;
}

void EditMode::SetPlayer(int& i, int& j, int count)
{
	// 実装
	auto& impl = (*pImpl_);

	// 床以外には置けない
	if(impl.mapData_[j][i] != FLOOR){
		return;
	}

	// パネルリスト
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// パネル光る
	panels[count]->SetColor(ideaColor::ORANGE);

	// 左ボタンが押されていればプレイヤーを置く
	if(impl.mouse_.GetButton(0) == 1){
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

		auto& player = (*GetGameObject<Player>(PLAYER_LAYER));
		player.SetPosition(pos);
		player.SetCell(j, i);
	}

	// 2重ループを抜ける
	i = impl.col_;
	j = impl.row_;
}

void EditMode::SetVita(int& i, int& j, int count)
{
	// 実装
	auto& impl = (*pImpl_);

	// 床以外には置けない
	if(impl.mapData_[j][i] != FLOOR){
		return;
	}

	// パネルリスト
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// パネル光る
	panels[count]->SetColor(ideaColor::ORANGE);

	// 左ボタンが押されていればvitaを置く
	if(impl.mouse_.GetButton(0) == 1){
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

		auto& vita = (*GetGameObject<Vita>(VITA_LAYER));
		vita.SetPosition(pos);
		vita.SetCell(j, i);
	}

	// 2重ループを抜ける
	i = impl.col_;
	j = impl.row_;
}

bool EditMode::RemoveBlock(idea::Camera& camera)
{
	// 実装
	auto& impl = (*pImpl_);

	// ブロックリスト
	auto blocks = GetGameObjects<Block>(BLOCK_LAYER);

	// 削除したか
	bool isRemove = false;

	for(auto& block : blocks){
		// マウスに重なっていなければ終了
		if(!impl.mouse_.CheckHitRayToSphere(camera, block->GetCube())){
			continue;
		}

		// パネル光る
		block->SetColor(ideaColor::ORANGE);

		// 右クリックしてなければ終了
		if(impl.mouse_.GetButton(1) == 0){
			return false;
		}

		// つながっている坂も一緒に消す
		for(int k = 0; k < 3; ++k){
			for(int l = 0; l < 3; ++l){
				if(k != 1 && l != 1){
					continue;
				}

				// 上下左右のセル座標
				int x = block->GetCell().y + k - 1;
				int z = block->GetCell().x + l - 1;

				if(x < 0 || impl.col_ < x){
					continue;
				}

				if(z < 0 || impl.row_ < z){
					continue;
				}

				// 坂でないなら無視
				if(impl.mapData_[z][x] == FLOOR
					|| impl.mapData_[z][x] == PLAYER
					|| impl.mapData_[z][x] == VITA){
					continue;
				}

				// 消去する坂のセル
				Cell deleteCell = Cell(z, x);

				for(auto& slope : blocks){
					if(slope->GetCell() != deleteCell){
						continue;
					}

					switch(slope->GetStyle()){
						case BlockStyle::UpperRight:
							if(k == 0){
								slope->SetDestroy();
							}
							break;

						case BlockStyle::UpperLeft:
							if(k == 2){
								slope->SetDestroy();
							}
							break;

						case BlockStyle::UpperBack:
							if(l == 2){
								slope->SetDestroy();
							}
							break;

						case BlockStyle::UpperFront:
							if(l == 0){
								slope->SetDestroy();
							}
							break;
					}
				}
			}

			block->SetDestroy();

			isRemove = true;
		}

		impl.isClick_ = true;

		break;
	}

	return isRemove;
}

void EditMode::UpdateMapData()
{
	// 実装
	auto& impl = (*pImpl_);

	// ブロックリスト
	auto blocks = GetGameObjects<Block>(BLOCK_LAYER);

	// マップデータ更新
	impl.mapData_.Clear();

	impl.mapData_.Reserve(impl.col_, impl.row_, 0);

	for(auto& block : blocks){
		// ブロックの存在するセル
		Cell cell = block->GetCell();

		// 範囲外のブロックを強制的に消す
		if(cell.x > impl.col_
			|| cell.y > impl.row_){
			block->SetDestroy();

			continue;
		}

		// ブロックの位置情報格納
		switch(block->GetStyle()){
			case BlockStyle::Box:
				impl.mapData_[cell.x][cell.y] = BOX;
				break;

			case BlockStyle::UpperRight:
				impl.mapData_[cell.x][cell.y] = UPPER_RIGHT;
				break;

			case BlockStyle::UpperLeft:
				impl.mapData_[cell.x][cell.y] = UPPER_LEFT;
				break;

			case BlockStyle::UpperBack:
				impl.mapData_[cell.x][cell.y] = UPPER_BACK;
				break;

			case BlockStyle::UpperFront:
				impl.mapData_[cell.x][cell.y] = UPPER_FRONT;
				break;
		}
	}

	{
		// プレイヤーの位置情報格納
		auto player = GetGameObject<Player>(PLAYER_LAYER);
		Cell cell = player->GetCell();
		impl.mapData_[cell.x][cell.y] = PLAYER;
	}

	{
		// vitaの位置情報格納
		auto vita = GetGameObject<Vita>(VITA_LAYER);
		Cell cell = vita->GetCell();
		impl.mapData_[cell.x][cell.y] = VITA;
	}
}

void EditMode::WriteMapData()
{
	// 実装
	auto& impl = (*pImpl_);

	// リスト用ID
	vector<const char*> id;

	// リスト用キー
	vector<const char*> key;

	// IDの書き込み
	for(int i = 0; i < (int)(impl.mapData_.GetRow()); ++i){
		// ID情報の格納
		ostringstream oss;
		char* c = new char[256];

		sprintf_s(c, 255, "%02d", i);

		id.push_back(c);
	}

	// キーの書き込み
	for(int i = 0; i < (int)impl.mapData_.GetCol(); ++i){
		// ー情報の格納
		ostringstream oss;
		char* c = new char[256];

		sprintf_s(c, 255, "%02d", i);

		key.push_back(c);
	}

	CsvWriter::WriteData<int>("map/edit_map_data.csv", impl.mapData_, id, key);

	for(auto i : id){
		SafeDeleteArray(i);
	}

	for(auto k : key){
		SafeDeleteArray(k);
	}
}
