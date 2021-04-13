#include "../../h/AI/CellAStar.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaUtility.h"

using namespace idea;

static const Cell CELL_DIRECTION[] = {
	{  0,  1 },
	{ -1,  1 },
	{ -1,  0 },
	{ -1, -1 },
	{  0, -1 },
	{  1, -1 },
	{  1,  0 },
	{  1,  1 } };

CellAStar::~CellAStar()
{
	for(auto& m : map_){
		delete m.second;
	}
}

void CellAStar::LoadMap(const char* pFileName)
{
	CsvReader crMap;

	crMap.LoadData(pFileName);

	row_ = crMap.GetRow();
	col_ = crMap.GetCol();

	for(int i = 0; i < row_; ++i){
		MapRow* map = new MapRow();

		for(int j = 0; j < col_; ++j){
			CellAStarNode n;
			n.status = crMap[i][j];

			map->Add(j, n);
		}

		map_[i] = map;
	}
}

void idea::CellAStar::LoadMap(idea::DynamicDualArray<int>* pArrey)
{
	row_ = pArrey->GetRow();
	col_ = pArrey->GetCol();

	for(int i = 0; i < row_; ++i){
		MapRow* map = new MapRow();

		for(int j = 0; j < col_; ++j){
			CellAStarNode n;
			n.status = (*pArrey)[i][j];

			map->Add(j, n);
		}

		map_[i] = map;
	}
}

void CellAStar::UnLoadMap()
{
	if(!map_.size()){
		return;
	}

	for(auto& m : map_){
		delete m.second;
	}

	std::unordered_map<int, MapRow*>().swap(map_);

	start_ = gorl_ = next_ = { -1, -1 };

	std::vector<Cell>().swap(root_);
}

bool CellAStar::Search(Cell start, Cell gorl)
{
	return Search(start.x, start.y, gorl.x, gorl.y);
}

bool CellAStar::Search(int startX, int startY, int gorlX, int gorlY)
{
	// �}�b�v��񂪖����Ȃ玸�s
	if(map_.size() == 0){
		next_ = { startX, startY };

		return false;
	}

	// �X�^�[�g�ƃS�[�����ꏏ�Ȃ�ΒT���̕K�v�Ȃ�
	if(Cell(startX, startY) == Cell(gorlX, gorlY)){
		next_ = { startX, startY };

		return true;
	}

	// �X�^�[�g�ƃS�[�����O��ς���Ă��Ȃ���ΒT���̕K�v�Ȃ�
	if(start_ == Cell(startX, startY) && gorl_ == Cell(gorlX, gorlY)){
		return true;
	}

	next_ = start_ = { startX, startY };

	// �X�^�[�g���͈͊O�Ȃ玸�s
	if(!IsInRange(startX, 0, col_ - 1)){
		return false;
	}

	if(!IsInRange(startY, 0, row_ - 1)){
		return false;
	}

	// �S�[�����͈͊O�Ȃ玸�s
	if(!IsInRange(gorlX, 0, col_ - 1)){
		return false;
	}

	if(!IsInRange(gorlY, 0, row_ - 1)){
		return false;
	}

	// �S�[�����N�����ׂȂ玸�s
	if(((*map_[gorlY])[gorlX]).status == 1){
		gorl_ = start_;

		return false;
	}

	gorl_ = { gorlX, gorlY };

	std::vector<Cell>().swap(root_);

	ResetSearchStatus();

	((*map_[start_.y])[start_.x]).state = CheckState::OPEN;

	// �T���s�\�Ȃ玸�s
	if(!Search()){
		next_ = start_;

		return false;
	}

	TraceRoute(gorl_.x, gorl_.y);

	std::reverse(root_.begin(), root_.end());

	return true;
}

bool CellAStar::Search(Cell start, Cell gorl, Cell& next)
{
	return Search(start.x, start.y, gorl.x, gorl.y, next);
}

bool CellAStar::Search(int startX, int startY, int gorlX, int gorlY, Cell& next)
{
	bool ret = Search(startX, startY, gorlX, gorlY);

	next = next_;

	return ret;
}

bool CellAStar::Search()
{
	int costMax = INT_MAX;

	Cell c;

	CellAStarNode* n = nullptr;

	// �R�X�g���ŏ��̃I�[�v���m�[�h��T��
	for(int i = row_ - 1; i >= 0; --i){
		for(int j = col_ - 1; j >= 0; --j){
			auto& map = ((*map_[i])[j]);

			if(map.state != CheckState::OPEN){
				continue;
			}

			// �q���[���b�N�R�X�g�̑���
			int cost = BackTrace(j, i, 0) + GetDistance(j, i, gorl_.x, gorl_.y);

			if(cost > costMax){
				continue;
			}

			costMax = cost;

			c = { j, i };

			n = &map;
		}
	}

	// �I�[�v���m�[�h���Ȃ���ΏI��(�����ӂ�����)
	if(!n){
		next_ = start_;
		return false;
	}

	// �m�[�h���N���[�Y����
	n->state = CheckState::CLOSE;

	for(int i = 8 - 1; i >= 0; --i){
		Cell check = { c.x + CELL_DIRECTION[i].x, c.y + CELL_DIRECTION[i].y };

		if(!IsInRange(check.x, 0, col_ - 1)){
			continue;
		}

		if(!IsInRange(check.y, 0, row_ - 1)){
			continue;
		}

		CellAStarNode& cell = ((*map_[check.y])[check.x]);

		// �ʍs�\��
		if(cell.status == 1){
			continue;
		}

		if(cell.state == CheckState::NO_CHECK){
			// �S�[���ɂ��悤�ɕ�����ݒ�
			cell.parent = (i + 4) % 8;

			// �S�[���ɒ�������T���I��
			if(check == gorl_){
				cell.state = CheckState::CLOSE;
				return true;
			}

			cell.state = CheckState::OPEN;
		}
	}

	return Search();
}

void CellAStar::ResetSearchStatus()
{
	for(int i = row_ - 1; i >= 0; --i){
		for(int j = col_ - 1; j >= 0; --j){
			auto& map = ((*map_[i])[j]);
			map.state = CheckState::NO_CHECK;
			map.parent = 0;
		}
	}
}

int CellAStar::BackTrace(int x, int y, int count)
{
	if(count >= 10000){
		return 10000;
	}

	// �X�^�[�g�ɒ�������T���I��
	if(x == start_.x && y == start_.y){
		return 1;
	}

	int direction = ((*map_[y])[x]).parent;

	return BackTrace(
		x + CELL_DIRECTION[direction].x,
		y + CELL_DIRECTION[direction].y,
		count + 1
	) + 1;
}

int CellAStar::GetDistance(int fromX, int fromY, int toX, int toY)
{
	return Abs(fromX - toX) + Abs(fromY - toY);
}

void CellAStar::TraceRoute(int x, int y)
{
	if(x == start_.x && y == start_.y){
		return;
	}

	next_.x = x;
	next_.y = y;

	root_.push_back(next_);

	int direction = ((*map_[y])[x]).parent;

	TraceRoute(
		x + CELL_DIRECTION[direction].x,
		y + CELL_DIRECTION[direction].y);
}

CellAStar::CellAStarNode& CellAStar::MapRow::operator[](int col)
{
	return value_[col];
}

void CellAStar::MapRow::Add(int col, CellAStarNode val)
{
	value_[col] = val;
}
