#ifndef INCLUDE_IDEA_CELLASTAR_H
#define INCLUDE_IDEA_CELLASTAR_H

#include "../Utility/CsvReader.h"
#include "../Utility/DynamicDualArray.h"

namespace idea{
	// 整数座標
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

		// マップデータの読み込み
		void LoadMap(const char* pFileName);

		// マップデータの読み込み
		void LoadMap(idea::DynamicDualArray<int>* pArrey);

		// マップデータの破棄
		void UnLoadMap();

		// 検索
		bool Search(Cell start, Cell gorl);

		// 検索
		bool Search(int startX, int startY, int gorlX, int gorlY);

		// 検索
		bool Search(Cell start, Cell gorl, Cell& next);

		// 検索
		bool Search(int startX, int startY, int gorlX, int gorlY, Cell& next);

		// 次に進むべきセルの取得
		const Cell& GetNext()const
		{
			return next_;
		}

		// ルートリストの取得
		std::vector<Cell> GetRoute()const
		{
			return root_;
		}

	private:
		// チェック情報
		enum class CheckState{
			NO_CHECK,
			OPEN,
			CLOSE,
		};

		// ノード情報
		struct CellAStarNode{
			// 移動可能か
			int status = 0;

			// 状態
			CheckState state = CheckState::NO_CHECK;

			// 親の向き
			int parent = 0;
		};

		// マップの行
		class MapRow{
		public:
			CellAStarNode& operator[](int col);

			// 値の追加
			void Add(int col, CellAStarNode val);

		private:
			// 値
			std::unordered_map<int, CellAStarNode> value_;
		};

		// 検索
		bool Search();

		// 検索情報の初期化
		void ResetSearchStatus();

		// 回帰コストの取得
		int BackTrace(int x, int y, int count);

		// 距離コストの取得
		int GetDistance(int fromX, int fromY, int toX, int toY);

		// ルートの割り出し
		void TraceRoute(int x, int y);

		// スタートの位置
		Cell start_ = {};

		// ゴールの位置
		Cell gorl_ = {};

		// 次に進むべきセル
		Cell next_ = {};

		// 行
		int row_ = 0;

		// 列
		int col_ = 0;

		// ルートリスト
		std::vector<Cell> root_;

		// マップ情報
		std::unordered_map<int, MapRow*> map_;
	};
}

#endif	// #ifndef INCLUDE_IDEA_CELLASTAR_H