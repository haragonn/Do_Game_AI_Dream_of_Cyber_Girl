#ifndef INCLUDE_IDEA_CELLASTAR_H
#define INCLUDE_IDEA_CELLASTAR_H

#include "../Utility/CsvReader.h"
#include "../Utility/DynamicDualArray.h"

namespace idea{
	// �������W
	struct Cell{
		int x;
		int y;

		Cell() :
			x(0),
			y(0){}

		Cell(int x, int y) :
			x(x),
			y(y){}

		bool operator==(const Cell& ref)
		{
			return ((x == ref.x) && (y == ref.y));
		}

		bool operator!=(const Cell& ref)
		{
			return ((x != ref.x) || (y != ref.y));
		}
	};

	class CellAStar{
	public:
		CellAStar() :
			start_({ -1, -1 }),
			gorl_({ -1, -1 }),
			next_({ -1, -1 }){}

		~CellAStar();

		// �}�b�v�f�[�^�̓ǂݍ���
		void LoadMap(const char* pFileName);

		// �}�b�v�f�[�^�̓ǂݍ���
		void LoadMap(idea::DynamicDualArray<int>* pArrey);

		// �}�b�v�f�[�^�̔j��
		void UnLoadMap();

		// ����
		bool Search(Cell start, Cell gorl);

		// ����
		bool Search(int startX, int startY, int gorlX, int gorlY);

		// ����
		bool Search(Cell start, Cell gorl, Cell& next);

		// ����
		bool Search(int startX, int startY, int gorlX, int gorlY, Cell& next);

		// ���ɐi�ނׂ��Z���̎擾
		const Cell& GetNext()const
		{
			return next_;
		}

		// ���[�g���X�g�̎擾
		std::vector<Cell> GetRoute()const
		{
			return root_;
		}

	private:
		// �`�F�b�N���
		enum class CheckState{
			NO_CHECK,
			OPEN,
			CLOSE,
		};

		// �m�[�h���
		struct CellAStarNode{
			// �ړ��\��
			int status = 0;

			// ���
			CheckState state = CheckState::NO_CHECK;

			// �e�̌���
			int parent = 0;
		};

		// �}�b�v�̍s
		class MapRow{
		public:
			CellAStarNode& operator[](int col);

			// �l�̒ǉ�
			void Add(int col, CellAStarNode val);

		private:
			// �l
			std::unordered_map<int, CellAStarNode> value_;
		};

		// ����
		bool Search();

		// �������̏�����
		void ResetSearchStatus();

		// ��A�R�X�g�̎擾
		int BackTrace(int x, int y, int count);

		// �����R�X�g�̎擾
		int GetDistance(int fromX, int fromY, int toX, int toY);

		// ���[�g�̊���o��
		void TraceRoute(int x, int y);

		// �X�^�[�g�̈ʒu
		Cell start_ = {};

		// �S�[���̈ʒu
		Cell gorl_ = {};

		// ���ɐi�ނׂ��Z��
		Cell next_ = {};

		// �s
		int row_ = 0;

		// ��
		int col_ = 0;

		// ���[�g���X�g
		std::vector<Cell> root_;

		// �}�b�v���
		std::unordered_map<int, MapRow*> map_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_CELLASTAR_H