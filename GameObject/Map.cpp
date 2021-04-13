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
	// ��
	static const int FLOOR = 0;
	
	// ��
	static const int BOX = 1;

	// �E�オ��̍�
	static const int UPPER_RIGHT = 2;

	// ���オ��̍�
	static const int UPPER_LEFT = 3;

	// ���オ��̍�
	static const int UPPER_BACK = 4;

	// ��O�オ��̍�
	static const int UPPER_FRONT = 5;
	
	// �v���C���[
	static const int PLAYER = 6;
	
	// vita
	static const int VITA = 7;
};

class Map::Impl{
public:
	// �}�b�v�f�[�^
	CsvReader mapData_;

	// �s
	int row_ = 0;

	// ��
	int col_ = 0;

	// �X�J�C�h�[��
	Sphere skyDome_;

	// ��
	PlaneMesh plane_;

	// ���̃e�N�X�`���[
	Texture planeTexture_;

	// �E�F�C�|�C���g���X�g
	WayPoints wayPoints_;

	// �m�[�h���Ȃ����C���̃��X�g
	vector<LineMesh*> lines_;

	// �l�ۊǌ�
	Memory memory_;

	// �f�o�b�O���
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
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.wayPoints_.Init(&(renderer.GetWrapCamera()));

	// �Q�[�����[�h�ɂ���ă��[�h����}�b�v�f�[�^��ς���
	if(impl.memory_.GetInt("game_mode") == MODE_ID_EDIT_GAME
		|| typeid(scene) == typeid(Preview)){
		impl.mapData_.LoadData("map/edit_map_data.csv");
	} else{
		impl.mapData_.LoadData("map/map_data.csv");
	}

	impl.row_ = impl.mapData_.GetRow();
	impl.col_ = impl.mapData_.GetCol();

	// �n��
	CreatePlane();

	// �X�J�C�h�[��
	CreateSkyDome();

	// �}�b�v����
	CreateBlock();

	// �m�[�h�̐ڑ�
	ConnectNode();

	// ��
	CreateWall();

	// �Q�[�����[�h�ɂ���ă��[�h����O���t�f�[�^��ς���
	// �v���r���[���[�h�̏ꍇ�̓O���t�f�[�^����������
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
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	for(auto& line : impl.lines_){
		SafeDelete(line);
	}

	// �O���t�f�[�^����������
	if(typeid(scene) == typeid(Preview)){
		impl.wayPoints_.WriteGraph("map/edit_graph_data.csv");
	}
}

void Map::Update()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �v���C���[
	auto& player = (*scene.GetGameObject<Player>(PLAYER_LAYER));

	// vita���X�g
	auto vitas = scene.GetGameObjects<Vita>(VITA_LAYER);

	// �A�C�e�����X�g
	auto items = scene.GetGameObjects<Item>(ITEM_LAYER);

	// �v���C���[�̃Z���̐ݒ�
	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// X���W
			float x = CenterAlign(0.0f, (float)(impl.col_ - 1), i, impl.col_);

			if(Abs(player.GetPosition().x - x) < (5.0f)){
				// Z���W
				float z = CenterAlign(0.0f, (float)(impl.row_ - 1), j, impl.row_);

				if(Abs(player.GetPosition().z - z) < (5.0f)){
					player.SetCell(i, impl.row_ - 1 - j);

					// 2�d���[�v�𔲂���
					i = impl.col_;
					break;
				}
			}
		}
	}

	// vita�����̃Z���̐ݒ�
	for(auto& vita : vitas){
		for(int i = 0; i < impl.col_; ++i){
			for(int j = 0; j < impl.row_; ++j){
				// X���W
				float x = CenterAlign(0.0f, (float)(impl.col_ - 1), i, impl.col_);

				if(Abs(vita->GetPosition().x - x) < (5.0f)){
					// Z���W
					float z = CenterAlign(0.0f, (float)(impl.row_ - 1), j, impl.row_);

					if(Abs(vita->GetPosition().z - z) < (5.0f)){
						vita->SetCell(i, impl.row_ - 1 - j);

						// 2�d���[�v�𔲂���
						i = impl.col_;
						break;
					}
				}
			}
		}
	}


	// item�����̃Z���̐ݒ�
	for(auto& item : items){
		for(int i = 0; i < impl.col_; ++i){
			for(int j = 0; j < impl.row_; ++j){
				// X���W
				float x = CenterAlign(0.0f, (float)(impl.col_ - 1), i, impl.col_);

				if(Abs(item->GetPosition().x - x) < (5.0f)){
					// Z���W
					float z = CenterAlign(0.0f, (float)(impl.row_ - 1), j, impl.row_);

					if(Abs(item->GetPosition().z - z) < (5.0f)){
						item->SetCell(i, impl.row_ - 1 - j);

						// 2�d���[�v�𔲂���
						i = impl.col_;
						break;
					}
				}
			}
		}
	}

	// �f�o�b�O�\��
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
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �����_���[
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
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	impl.skyDome_.Init(0, 0, 0);
	impl.skyDome_.SetScale(300, 300, 300);
	impl.skyDome_.SetTexture(impl.planeTexture_);
	impl.skyDome_.SetColor(ideaColor::CYAN);
	renderer.AddObject(impl.skyDome_);
}

void Map::CreateBlock()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// X���W
			float x = CenterAlign(0.0f, 10.0f, i, impl.col_);

			// Z���W
			float z = CenterAlign(0.0f, 10.0f, (impl.row_ - 1) - j, impl.row_);

			// �E�F�C�|�C���g�̍��W
			Vector3D waypointPosition(x, 0.0f, z);

			// ���̃E�F�C�|�C���g�ݒu
			if((int)impl.mapData_[j][i] == FLOOR
				|| (int)impl.mapData_[j][i] == PLAYER
				|| (int)impl.mapData_[j][i] == VITA){
				impl.wayPoints_.AddWayPoint(waypointPosition.ReplaceY(0.5f));

				continue;
			}

			// �u���b�N�̐ݒu
			auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));

			// �u���b�N�̍��W
			Vector3D blockPosition(x, 0.0f, z);
			block.SetPosition(blockPosition);

			// �u���b�N�̃E�F�C�|�C���g�ݒu
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
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// �����_���[
	auto& renderer = (*scene.GetGameObject<Renderer>(RENDERER_LAYER));

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			if(i != impl.col_ - 1){
				// �m�[�hID1
				int nodeID1 = i * impl.row_ + j;

				// �m�[�hID2
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
					// �m�[�hID1
					int nodeID1 = i * impl.row_ + j;

					// �m�[�hID2
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
					// �m�[�hID1
					int nodeID1 = i * impl.row_ + j;

					// �m�[�hID2
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
					// �m�[�hID1
					int nodeID1 = i * impl.row_ + j + 1;

					// �m�[�hID2
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

	// �u���b�N���X�g
	auto blocks = scene.GetGameObjects<Block>(BLOCK_LAYER);

	// �m�[�h�����C���łȂ�
	for(auto point : impl.wayPoints_.GetWayPoints()){
		for(auto node : point->GetNodes()){
			// ���C�����b�V��
			LineMesh* line = new LineMesh;

			line->Create(0.1f);
			line->SetPoint(point->GetPosition(), node->GetPosition());

			// ��Q�����Փ˂��Ă��邩
			bool isHit = false;

			for(auto& block : blocks){
				if(line->GetCollider().CheckHit(block->GetCollider())){
					impl.wayPoints_.DisconnectWayPoint(point, node);

					isHit = true;

					break;
				}
			}

			if(!isHit){
				// ���X�g�ɒǉ�
				impl.lines_.push_back(line);
				renderer.GetWrapCamera().AddObject(*line);
			} else{
				// ��Q���ƏՓ˂��Ă��郉�C���͏���
				SafeDelete(line);
			}
		}
	}
}

void Map::CreateWall()
{
	// ����
	auto& impl = (*pImpl_);

	// �V�[��
	auto& scene = (*pScene_);

	// ��
	{
		// X���W
		float x = CenterAlign(0.0f, 10.0f, impl.col_ + 1, impl.col_ + 2);

		// Z���W
		float z = CenterAlign(0.0f, 10.0f, impl.row_ / 2, impl.row_);

		if(impl.row_ % 2 == 0){
			z -= 5.0f;
		}

		// �u���b�N�̍��W
		Vector3D blockPosition(x + 10.0f, 10.0f, z);

		// �u���b�N�̐���
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.row_ + 3));
		block.SetStyle(BlockStyle::Wall1);
	}

	// ��
	{
		// X���W
		float x = CenterAlign(0.0f, 10.0f, 0, impl.col_ + 2);

		// Z���W
		float z = CenterAlign(0.0f, 10.0f, impl.row_ / 2, impl.row_);

		if(impl.row_ % 2 == 0){
			z -= 5.0f;
		}

		// �u���b�N�̍��W
		Vector3D blockPosition(x - 10.0f, 10.0f, z);

		// �u���b�N�̐���
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.row_ + 4));
		block.SetStyle(BlockStyle::Wall1);
	}

	// ��
	{
		// X���W
		float x = CenterAlign(0.0f, 10.0f, impl.col_ / 2, impl.col_);

		// Z���W
		float z = CenterAlign(0.0f, 10.0f, 0, impl.row_ + 2);

		if(impl.col_ % 2 == 0){
			x -= 5.0f;
		}

		// �u���b�N�̍��W
		Vector3D blockPosition(x, 10.0f, z - 10.0f);

		// �u���b�N�̐���
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.col_ + 4));
		block.SetStyle(BlockStyle::Wall2);
	}


	// �k
	{
		// X���W
		float x = CenterAlign(0.0f, 10.0f, impl.col_ / 2, impl.col_);

		// Z���W
		float z = CenterAlign(0.0f, 10.0f, impl.row_ + 1, impl.row_ + 2);

		if(impl.col_ % 2 == 0){
			x -= 5.0f;
		}

		// �u���b�N�̍��W
		Vector3D blockPosition(x, 10.0f, z + 10.0f);

		// �u���b�N�̐���
		auto& block = (*scene.AddGameObject<Block>(BLOCK_LAYER));
		block.SetPosition(blockPosition);
		block.SetScale(30.0f, 20.0f, 10.0f * (impl.col_ + 4));
		block.SetStyle(BlockStyle::Wall2);
	}
}
