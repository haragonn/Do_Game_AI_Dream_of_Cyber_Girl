#include "Map.h"
#include "../Scene/MainGame.h"
#include "../Scene/Preview.h"
#include "Renderer.h"
#include "Player.h"
#include "Vita.h"
#include "Item.h"
#include "Block.h"

using namespace std;
using namespace idea;

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

class Map::Impl{
public:
	// マップデータ
	CsvReader mapData_;

	// 行
	int row_ = 0;

	// 列
	int col_ = 0;

	// スカイドーム
	Sphere skyDome_;

	// 床
	PlaneMesh plane_;

	// 床のテクスチャー
	Texture planeTexture_;

	// ウェイポイントリスト
	WayPoints wayPoints_;

	// ノードをつなぐラインのリスト
	vector<LineMesh*> lines_;

	// 値保管庫
	Memory memory_;

	// デバッグ情報
	Debug debugLog_;
};

Map::Map() :
	pImpl_(new Map::Impl)
{
	Assert(pImpl_);
}

Map::~Map()
{
	SafeDelete(pImpl_);
}

void Map::Init()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.wayPoints_.Init(&(renderer.GetWrapCamera()));

	// ゲームモードによってロードするマップデータを変える
	if(impl.memory_.GetInt("game_mode") == MODE_ID_EDIT_GAME
		|| typeid(scene) == typeid(Preview)){
		impl.mapData_.LoadData("map/edit_map_data.csv");
	} else{
		impl.mapData_.LoadData("map/map_data.csv");
	}

	impl.row_ = impl.mapData_.GetRow();
	impl.col_ = impl.mapData_.GetCol();

	// 地面
	CreatePlane();

	// スカイドーム
	CreateSkyDome();

	// マップ生成
	CreateBlock();

	// ノードの接続
	ConnectNode();

	// 壁
	CreateWall();

	// ゲームモードによってロードするグラフデータを変える
	// プレビューモードの場合はグラフデータをかきこむ
	if(typeid(scene) == typeid(Preview)){
		impl.wayPoints_.Analyze();
		impl.wayPoints_.WriteGraph("map/edit_graph_data.csv");
	} else if(impl.memory_.GetInt("game_mode") == MODE_ID_EDIT_GAME){
		impl.wayPoints_.LoadGraph("map/edit_graph_data.csv");
	} else{
		impl.wayPoints_.LoadGraph("map/graph_data.csv");
	}

	auto player = scene.AddGameObject<Player>(PLAYER_LAYER);
	auto vita = scene.AddGameObject<Vita>(VITA_LAYER);

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			float x = CenterAlign(0.0f, 10.0f, i, impl.col_);
			float z = CenterAlign(0.0f, 10.0f, (impl.row_ - 1) - j, impl.row_);

			Vector3D position(x, 0.0f, z);

			if((int)impl.mapData_[j][i] == 6){
				player->SetPosition(position);
			} else if((int)impl.mapData_[j][i] == 7){
				vita->SetPosition(position);
			}
		}
	}

	if(impl.debugLog_.IsDebugMode()){
		impl.debugLog_.SetDebugSwitch(1, true);
	}
}

void Map::UnInit()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	for(auto& line : impl.lines_){
		SafeDelete(line);
	}

	// グラフデータをかきこむ
	if(typeid(scene) == typeid(Preview)){
		impl.wayPoints_.WriteGraph("map/edit_graph_data.csv");
	}
}

void Map::Update()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// プレイヤー
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vitaリスト
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// アイテムリスト
	auto items = scene.GetGameObjects<Item>(ITEM_LAYER);

	// プレイヤーのセルの設定
	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// X座標
			float x = CenterAlign(0.0f, (float)(impl.col_ - 1), i, impl.col_);

			if(Abs(player.GetPosition().x - x) < (5.0f)){
				// Z座標
				float z = CenterAlign(0.0f, (float)(impl.row_ - 1), j, impl.row_);

				if(Abs(player.GetPosition().z - z) < (5.0f)){
					player.SetCell(i, impl.row_ - 1 - j);

					// 2重ループを抜ける
					i = impl.col_;
					break;
				}
			}
		}
	}

	// vitaたちのセルの設定
	for(auto& vita : vitas){
		for(int i = 0; i < impl.col_; ++i){
			for(int j = 0; j < impl.row_; ++j){
				// X座標
				float x = CenterAlign(0.0f, (float)(impl.col_ - 1), i, impl.col_);

				if(Abs(vita->GetPosition().x - x) < (5.0f)){
					// Z座標
					float z = CenterAlign(0.0f, (float)(impl.row_ - 1), j, impl.row_);

					if(Abs(vita->GetPosition().z - z) < (5.0f)){
						vita->SetCell(i, impl.row_ - 1 - j);

						// 2重ループを抜ける
						i = impl.col_;
						break;
					}
				}
			}
		}
	}


	// itemたちのセルの設定
	for(auto& item : items){
		for(int i = 0; i < impl.col_; ++i){
			for(int j = 0; j < impl.row_; ++j){
				// X座標
				float x = CenterAlign(0.0f, (float)(impl.col_ - 1), i, impl.col_);

				if(Abs(item->GetPosition().x - x) < (5.0f)){
					// Z座標
					float z = CenterAlign(0.0f, (float)(impl.row_ - 1), j, impl.row_);

					if(Abs(item->GetPosition().z - z) < (5.0f)){
						item->SetCell(i, impl.row_ - 1 - j);

						// 2重ループを抜ける
						i = impl.col_;
						break;
					}
				}
			}
		}
	}

	// デバッグ表示
	if(impl.debugLog_.GetDebugSwitch(1) || typeid(scene) == typeid(Preview)){
		impl.wayPoints_.SetVisibleFlag(true);

		for(auto& line : impl.lines_){
			line->SetColor(ideaColor::WHITE);
		}
	} else{
		impl.wayPoints_.SetVisibleFlag(false);

		for(auto& line : impl.lines_){
			line->SetColor(ideaColor::CLEAR);
		}
	}

	impl.wayPoints_.ColorReset();
}

WayPoints& Map::GetWayPoints()
{
	return pImpl_->wayPoints_;
}

int Map::GetRow()
{
	return pImpl_->row_;
}

int Map::GetCol()
{
	return pImpl_->col_;
}

void Map::CreatePlane()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.planeTexture_.LoadImageFromStorage("asset/TEXTURE/grid05.bmp");

	impl.plane_.Init(0, 0, 0);
	impl.plane_.Create(0, 0, 10, 10, impl.col_, impl.row_);
	impl.plane_.SetTexture(impl.planeTexture_);
	impl.plane_.SetColor(ideaColor::SOFT_BLUE);
	renderer.AddObject(impl.plane_);
	impl.plane_.SetShadow(renderer.GetShadowCamera());
}

void Map::CreateSkyDome()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.skyDome_.Init(0, 0, 0);
	impl.skyDome_.SetScale(300, 300, 300);
	impl.skyDome_.SetTexture(impl.planeTexture_);
	impl.skyDome_.SetColor(ideaColor::CYAN);
	renderer.AddObject(impl.skyDome_);
}

void Map::CreateBlock()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// X座標
			float x = CenterAlign(0.0f, 10.0f, i, impl.col_);

			// Z座標
			float z = CenterAlign(0.0f, 10.0f, (impl.row_ - 1) - j, impl.row_);

			// ウェイポイントの座標
			Vector3D waypointPosition(x, 0.0f, z);

			// 床のウェイポイント設置
			if((int)impl.mapData_[j][i] == FLOOR
				|| (int)impl.mapData_[j][i] == PLAYER
				|| (int)impl.mapData_[j][i] == VITA){
				impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(0.5f));

				continue;
			}

			// ブロックの設置
			auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));

			// ブロックの座標
			Vector3D blockPosition(x, 0.0f, z);
			block.SetPosition(blockPosition);

			// ブロックのウェイポイント設置
			switch((int)impl.mapData_[j][i]){
				case BOX:
					block.SetStyle(BlockStyle::Box);
					impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(7.5f));
					break;

				case UPPER_RIGHT:
					block.SetStyle(BlockStyle::UpperRight);
					impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(2.0f).ReplaceX(waypointPosition.x - 2.0f), 200.0f);
					break;

				case UPPER_LEFT:
					block.SetStyle(BlockStyle::UpperLeft);
					impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(2.0f).ReplaceX(waypointPosition.x + 2.0f), 200.0f);
					break;

				case UPPER_BACK:
					block.SetStyle(BlockStyle::UpperBack);
					impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(2.0f).ReplaceZ(waypointPosition.z - 2.0f), 200.0f);
					break;

				case UPPER_FRONT:
					block.SetStyle(BlockStyle::UpperFront);
					impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(2.0f).ReplaceZ(waypointPosition.z + 2.0f), 200.0f);
					break;
			}
		}
	}

	impl.wayPoints_.SetVisibleFlag(false);
}

void Map::ConnectNode()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// レンダラー
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			if(i != impl.col_ - 1){
				// ノードID1
				int nodeID1 = i * impl.row_ + j;

				// ノードID2
				int nodeID2 = (i + 1) * impl.row_ + j;

				if(!impl.wayPoints_.GetWayPoint(nodeID1)){
					continue;
				}

				if(!impl.wayPoints_.GetWayPoint(nodeID2)){
					continue;
				}

				impl.wayPoints_.ConnectWayPoint(nodeID1, nodeID2);
			}

			if(j != impl.row_ - 1){
				{
					// ノードID1
					int nodeID1 = i * impl.row_ + j;

					// ノードID2
					int nodeID2 = i * impl.row_ + j + 1;

					if(!impl.wayPoints_.GetWayPoint(nodeID1)){
						continue;
					}

					if(!impl.wayPoints_.GetWayPoint(nodeID2)){
						continue;
					}

					impl.wayPoints_.ConnectWayPoint(nodeID1, nodeID2);
				}
				{
					// ノードID1
					int nodeID1 = i * impl.row_ + j;

					// ノードID2
					int nodeID2 = (i + 1) * impl.row_ + j + 1;

					if(!impl.wayPoints_.GetWayPoint(nodeID1)){
						continue;
					}

					if(!impl.wayPoints_.GetWayPoint(nodeID2)){
						continue;
					}

					impl.wayPoints_.ConnectWayPoint(nodeID1, nodeID2);
				}
				{
					// ノードID1
					int nodeID1 = i * impl.row_ + j + 1;

					// ノードID2
					int nodeID2 = (i + 1) * impl.row_ + j;

					if(!impl.wayPoints_.GetWayPoint(nodeID1)){
						continue;
					}

					if(!impl.wayPoints_.GetWayPoint(nodeID2)){
						continue;
					}

					impl.wayPoints_.ConnectWayPoint(nodeID1, nodeID2);
				}
			}
		}
	}

	// ブロックリスト
	auto blocks = scene.GetGameObjects<Block>(BLOCK_LAYER);

	// ノードをラインでつなぐ
	for(auto point : impl.wayPoints_.GetWayPoints()){
		for(auto node : point->GetNodes()){
			// ラインメッシュ
			LineMesh* line = new LineMesh;

			line->Create(0.1f);
			line->SetPoint(point->GetPosition(), node->GetPosition());

			// 障害物を衝突しているか
			bool isHit = false;

			for(auto& block : blocks){
				if(line->GetCollider().CheckHit(block->GetCollider())){
					impl.wayPoints_.DisconnectWayPoint(point, node);

					isHit = true;

					break;
				}
			}

			if(!isHit){
				// リストに追加
				impl.lines_.push_back(line);
				renderer.GetWrapCamera().AddObject(*line);
			} else{
				// 障害物と衝突しているラインは消去
				SafeDelete(line);
			}
		}
	}
}

void Map::CreateWall()
{
	// 実装
	auto& impl = (*pImpl_);

	// シーン
	auto& scene = (*pScene_);

	// 東
	{
		// X座標
		float x = CenterAlign(0.0f, 10.0f, impl.col_ + 1, impl.col_ + 2);

		// Z座標
		float z = CenterAlign(0.0f, 10.0f, impl.row_ / 2, impl.row_);

		if(impl.row_ % 2 == 0){
			z -= 5.0f;
		}

		// ブロックの座標
		Vector3D blockPosition(x + 10.0f, 10.0f, z);

		// ブロックの生成
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.row_ + 3));
		block.SetStyle(BlockStyle::Wall1);
	}

	// 西
	{
		// X座標
		float x = CenterAlign(0.0f, 10.0f, 0, impl.col_ + 2);

		// Z座標
		float z = CenterAlign(0.0f, 10.0f, impl.row_ / 2, impl.row_);

		if(impl.row_ % 2 == 0){
			z -= 5.0f;
		}

		// ブロックの座標
		Vector3D blockPosition(x - 10.0f, 10.0f, z);

		// ブロックの生成
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.row_ + 4));
		block.SetStyle(BlockStyle::Wall1);
	}

	// 南
	{
		// X座標
		float x = CenterAlign(0.0f, 10.0f, impl.col_ / 2, impl.col_);

		// Z座標
		float z = CenterAlign(0.0f, 10.0f, 0, impl.row_ + 2);

		if(impl.col_ % 2 == 0){
			x -= 5.0f;
		}

		// ブロックの座標
		Vector3D blockPosition(x, 10.0f, z - 10.0f);

		// ブロックの生成
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.col_ + 4));
		block.SetStyle(BlockStyle::Wall2);
	}


	// 北
	{
		// X座標
		float x = CenterAlign(0.0f, 10.0f, impl.col_ / 2, impl.col_);

		// Z座標
		float z = CenterAlign(0.0f, 10.0f, impl.row_ + 1, impl.row_ + 2);

		if(impl.col_ % 2 == 0){
			x -= 5.0f;
		}

		// ブロックの座標
		Vector3D blockPosition(x, 10.0f, z + 10.0f);

		// ブロックの生成
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.col_ + 4));
		block.SetStyle(BlockStyle::Wall2);
	}
}
