#include "EditMode.h"
#include "Preview.h"
#include "../GameObject/EditRenderer.h"
#include "../GameObject/Panel.h"
#include "../GameObject/Block.h"
#include "../GameObject/Player.h"
#include "../GameObject/Vita.h"

using namespace std;
using namespace idea;

// �}�b�v���ID
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

// �ݒu���[�h
enum class SetMode{
	Box,	// ��
	Slope,	// ��
	Player,	// �v���C���[
	Vita	// AI
};

// EditMode�̎���
class EditMode::Impl{
public:
	// �R���g���[���[����
	Controller controller_;

	// �}�E�X����
	Mouse mouse_;

	// ��
	PlaneMesh plane_;

	// ���̃e�N�X�`���[
	Texture texturePlane_;

	// �}�b�v�̍s��
	int row_ = 0;

	// �}�b�v�̗�
	int col_ = 0;

	// �}�b�v�f�[�^
	DynamicDualArray<int> mapData_;

	// �ݒu���[�h
	SetMode setMode_;

	// �N���b�N�t���O
	bool isClick_ = false;

	// BGM
	Sound bgm_;

	// BGM�̃{�����[��
	float bgmVolume_ = 0.0f;

	// ���艹
	Sound seDecision_;

	// �I���t���O
	bool isEnd_ = false;

	// ����@�\���p�t�H���g
	Font fontInfo_;

	// �ݒu���[�h�\���p�t�H���g
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
	// ����
	auto& impl = (*pImpl_);

	// �����_���[
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

	// �|�C���g����p�p�l���ݒu
	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// �p�l������
			auto& panel = (*AddGameObject<Panel>(PANEL_LAYER));

			// �|�W�V�����̐ݒ�
			Vector3D pos;
			pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
			pos.y = 0.0f;
			pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);
			panel.SetPosition(pos);

			panel.SetCell(i, j);

			renderer.AddObject(panel.GetCube());
		}
	}

	// �v���C���[�ݒu
	{
		// �v���C���[����
		auto& player = (*AddGameObject<Player>(PLAYER_LAYER));

		// �|�W�V�����̐ݒ�
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, 5, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, 7, impl.row_);
		player.SetPosition(pos);

		player.SetCell(7, 5);
	}

	// vita�ݒu
	{
		// ita����
		auto& vita = (*AddGameObject<Vita>(VITA_LAYER));

		// �|�W�V�����̐ݒ�
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

	impl.bgm_.LoadWaveFromStorage("�A�g���G�Ɠd�]���E_2.wav");
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
	// ����
	auto& impl = (*pImpl_);

	Scene::Update();

	// �V�[���J��
	if(fade_.IsFadeOutCompleted()){
		WriteMapData();

		gameManager_.SetNextScenePtr(new Preview);

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

	// Enter�L�[�����Ńt�F�[�h
	if(!fade_.IsFading()
		&& impl.controller_.GetButton(PadButton::START) == 1){
		impl.isEnd_ = true;

		fade_.SetFade(40);

		impl.seDecision_.Play();
	}

	// �z�C�[����]�Ń��[�h�`�F���W
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

	// �J����
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

	// ����
	auto& impl = (*pImpl_);

	// �������
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 0, "���N���b�N");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 1, "�ݒu");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 3, "�E�N���b�N");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 4, "����");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 6, "�z�C�[����]");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 7, "�ݒu�I�u�W�F�N�g�ؑ�");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 9, "���L�[");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 10, "�ړ�");

	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 12, "ENTER�L�[");
	impl.fontInfo_.DrawFormatText(0.0f, 24.0f * 13, "�I��");

	// �ݒu���[�h�\��
	switch(impl.setMode_){
		case SetMode::Box:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "BOX�Z�b�g");
			break;

		case SetMode::Slope:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "SLOPE�Z�b�g");
			break;

		case SetMode::Player:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "PLAYER�Z�b�g");
			break;

		case SetMode::Vita:
			impl.fontMode_.DrawFormatText(S_W - 40.0f * 6, 0.0f, "AI�Z�b�g");
			break;
	}
}

void EditMode::SetObject(idea::Camera& camera)
{
	// ����
	auto& impl = (*pImpl_);

	// �u���b�N������A�����Ĕ��肵�Ȃ�
	if(impl.isClick_){
		return;
	}

	// �p�l�����X�g
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// �p�l���̒ʂ��ԍ�
	int panelCount = 0;

	for(int i = 0; i < impl.col_; ++i){
		for(int j = 0; j < impl.row_; ++j){
			// �}�E�X�ɏd�Ȃ��Ă��Ȃ���ΏI��
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
	// ����
	auto& impl = (*pImpl_);

	// ���ȊO�ɂ͒u���Ȃ�
	if(impl.mapData_[j][i] != FLOOR){
		return false;
	}

	// ��̉��Ƀu���b�N�����Ȃ��悤�ɂ���
	{
		// �ݒu�\��
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

		// 2�d���[�v�𔲂���
		if(!canSet){
			i = impl.col_;
			j = impl.row_;

			return false;
		}
	}

	// �p�l�����X�g
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// �p�l������
	panels[count]->SetColor(ideaColor::ORANGE);

	// �ݒu�t���O
	bool isSet = false;

	// �N���b�N�Ŕ��ݒu
	if(impl.mouse_.GetButton(0)){
		// �|�W�V�����̐ݒ�
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

	// 2�d���[�v�𔲂���
	i = impl.col_;
	j = impl.row_;

	return isSet;
}

bool EditMode::SetSlope(int& i, int& j, int count)
{
	// ����
	auto& impl = (*pImpl_);

	// ���ȊO�ɂ͒u���Ȃ�
	if(impl.mapData_[j][i] != FLOOR){
		return false;
	}

	// �p�l�����X�g
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

			// ��̉����u���b�N�Ȃ���Ȃ�
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

			// ���ɗאڎ��̂ݐݒu�\
			if(impl.mapData_[x][z] == 1){
				// �p�l������
				panels[count]->SetColor(ideaColor::ORANGE);

				// ���{�^����������Ă���΍��u��
				if(impl.mouse_.GetButton(0)){
					auto& block = (*AddGameObject<Block>(BLOCK_LAYER));

					// �|�W�V�����̐ݒ�
					Vector3D pos;
					pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
					pos.y = 0.0f;
					pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

					block.SetPosition(pos);

					// ���Έʒu����`������
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

					// 2�d���[�v�𔲂���
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
	// ����
	auto& impl = (*pImpl_);

	// ���ȊO�ɂ͒u���Ȃ�
	if(impl.mapData_[j][i] != FLOOR){
		return;
	}

	// �p�l�����X�g
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// �p�l������
	panels[count]->SetColor(ideaColor::ORANGE);

	// ���{�^����������Ă���΃v���C���[��u��
	if(impl.mouse_.GetButton(0) == 1){
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

		auto& player = (*GetGameObject<Player>(PLAYER_LAYER));
		player.SetPosition(pos);
		player.SetCell(j, i);
	}

	// 2�d���[�v�𔲂���
	i = impl.col_;
	j = impl.row_;
}

void EditMode::SetVita(int& i, int& j, int count)
{
	// ����
	auto& impl = (*pImpl_);

	// ���ȊO�ɂ͒u���Ȃ�
	if(impl.mapData_[j][i] != FLOOR){
		return;
	}

	// �p�l�����X�g
	auto panels = GetGameObjects<Panel>(PANEL_LAYER);

	// �p�l������
	panels[count]->SetColor(ideaColor::ORANGE);

	// ���{�^����������Ă����vita��u��
	if(impl.mouse_.GetButton(0) == 1){
		Vector3D pos;
		pos.x = CenterAlign(0.0f, 10.0f, i, impl.col_);
		pos.y = 0.0f;
		pos.z = -CenterAlign(0.0f, 10.0f, j, impl.row_);

		auto& vita = (*GetGameObject<Vita>(VITA_LAYER));
		vita.SetPosition(pos);
		vita.SetCell(j, i);
	}

	// 2�d���[�v�𔲂���
	i = impl.col_;
	j = impl.row_;
}

bool EditMode::RemoveBlock(idea::Camera& camera)
{
	// ����
	auto& impl = (*pImpl_);

	// �u���b�N���X�g
	auto blocks = GetGameObjects<Block>(BLOCK_LAYER);

	// �폜������
	bool isRemove = false;

	for(auto& block : blocks){
		// �}�E�X�ɏd�Ȃ��Ă��Ȃ���ΏI��
		if(!impl.mouse_.CheckHitRayToSphere(camera, block->GetCube())){
			continue;
		}

		// �p�l������
		block->SetColor(ideaColor::ORANGE);

		// �E�N���b�N���ĂȂ���ΏI��
		if(impl.mouse_.GetButton(1) == 0){
			return false;
		}

		// �Ȃ����Ă������ꏏ�ɏ���
		for(int k = 0; k < 3; ++k){
			for(int l = 0; l < 3; ++l){
				if(k != 1 && l != 1){
					continue;
				}

				// �㉺���E�̃Z�����W
				int x = block->GetCell().y + k - 1;
				int z = block->GetCell().x + l - 1;

				if(x < 0 || impl.col_ < x){
					continue;
				}

				if(z < 0 || impl.row_ < z){
					continue;
				}

				// ��łȂ��Ȃ疳��
				if(impl.mapData_[z][x] == FLOOR
					|| impl.mapData_[z][x] == PLAYER
					|| impl.mapData_[z][x] == VITA){
					continue;
				}

				// ���������̃Z��
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
	// ����
	auto& impl = (*pImpl_);

	// �u���b�N���X�g
	auto blocks = GetGameObjects<Block>(BLOCK_LAYER);

	// �}�b�v�f�[�^�X�V
	impl.mapData_.Clear();

	impl.mapData_.Reserve(impl.col_, impl.row_, 0);

	for(auto& block : blocks){
		// �u���b�N�̑��݂���Z��
		Cell cell = block->GetCell();

		// �͈͊O�̃u���b�N�������I�ɏ���
		if(cell.x > impl.col_
			|| cell.y > impl.row_){
			block->SetDestroy();

			continue;
		}

		// �u���b�N�̈ʒu���i�[
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
		// �v���C���[�̈ʒu���i�[
		auto player = GetGameObject<Player>(PLAYER_LAYER);
		Cell cell = player->GetCell();
		impl.mapData_[cell.x][cell.y] = PLAYER;
	}

	{
		// vita�̈ʒu���i�[
		auto vita = GetGameObject<Vita>(VITA_LAYER);
		Cell cell = vita->GetCell();
		impl.mapData_[cell.x][cell.y] = VITA;
	}
}

void EditMode::WriteMapData()
{
	// ����
	auto& impl = (*pImpl_);

	// ���X�g�pID
	vector<const char*> id;

	// ���X�g�p�L�[
	vector<const char*> key;

	// ID�̏�������
	for(int i = 0; i < (int)(impl.mapData_.GetRow()); ++i){
		// ID���̊i�[
		ostringstream oss;
		char* c = new char[256];

		sprintf_s(c, 255, "%02d", i);

		id.push_back(c);
	}

	// �L�[�̏�������
	for(int i = 0; i < (int)impl.mapData_.GetCol(); ++i){
		// �[���̊i�[
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
