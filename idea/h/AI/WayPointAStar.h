#ifndef INCLUDE_IDEA_WAYPOINTASTAR_H
#define INCLUDE_IDEA_WAYPOINTASTAR_H

#include <vector>

#include "../3D/Sphere.h"
#include "../Utility/ideaMath.h"
#include "../Utility/DynamicDualArray.h"

namespace idea{
	// 座標
	class WayPoint : public Sphere{
	public:
		WayPoint();

		WayPoint(int id, Vector3D position, float cost);

		// IDの取得
		int GetID()const
		{
			return id_;
		}

		// ノードの追加
		void AddNode(WayPoint* node);

		// ノードの消去
		void RemoveNode(WayPoint* node);

		// ノードの取得
		std::vector<WayPoint*> GetNodes();

	private:
		// チェック情報
		enum class CheckState{
			NO_CHECK,
			OPEN,
			CLOSE,
		}state_;

		friend class WayPoints;

		// ID
		int id_ = 0;

		// コスト
		float cost_ = 0.0f;

		// ノードリスト
		std::vector<WayPoint*> nodes_;

		// 親ノード
		WayPoint* parent_ = nullptr;
	};

	class WayPoints{
	public:
		~WayPoints();

		// 初期化
		void Init(Camera* pCamera);

		// 終了処理
		void UnInit();

		// ウェイポイント情報の読み込み
		bool LoadWayPoint(const char* pFileName);

		// グラフ情報の読み込み
		bool LoadGraph(const char* pFileName);

		// ウェイポイント情報の書き出し
		bool WriteWayPoint(const char* pFileName);

		// グラフ情報の書き出し
		bool WriteGraph(const char* pFileName);

		// ウェイポイントの追加
		int AddWayPoint(const Vector3D& position, float cost = 0.0f);

		// ウェイポイントの消去
		void RemoveWayPoint(int id);

		// ウェイポイントの接続
		void ConnectWayPoint(int id1, int id2);

		// ウェイポイントの接続
		void ConnectWayPoint(WayPoint* pPoint1, WayPoint* pPoint2);

		// ウェイポイントの切断
		void DisconnectWayPoint(int id1, int id2);

		// ウェイポイントの接続
		void DisconnectWayPoint(WayPoint* pPoint1, WayPoint* pPoint2);

		// ウェイポイントの数の取得
		int GetWayPointSize()const;

		// ウェイポイントの取得
		WayPoint* GetWayPoint(int id);

		// ウェイポイントリストの取得
		std::vector<WayPoint*>& GetWayPoints();

		// グラフの取得
		DynamicDualArray<int>& GetGraph();

		// 最短経路検索
		bool Analyze();

		// 色の初期化
		void ColorReset();

		// 分析済みか
		bool IsAnalyze()const
		{
			return bAnalyze_;
		}

		// 可視か
		bool IsVisible()const
		{
			return bVisible_;
		}

		// 可視フラグの設定
		void SetVisibleFlag(bool bFlag);

	private:
		// 検索済みか
		bool bAnalyze_ = false;

		bool bVisible_ = false;

		// ウェイポイントリスト
		std::vector<WayPoint*> wayPoints_;

		Camera* pCamera_ = nullptr;

		// 行
		int row_ = 0;

		// 列
		int col_ = 0;

		// 最短経路
		DynamicDualArray<int> route_;

		// 経路ID
		std::vector<int> path_;

		// 最短経路グラフ
		DynamicDualArray<int> graph_;

		bool Analyze(WayPoint* node, WayPoint* node2);

		// 検索情報の初期化
		void ResetSearchStatus();

		// 回帰コストの取得
		int BackTrace(WayPoint* node, WayPoint* node2, int count);

		// 距離コストの取得
		float GetDistance(WayPoint* node, WayPoint* node2);

		// ルートの割り出し
		void TraceRoute(WayPoint* node, WayPoint* node2);

		// グラフ化
		void Graphing();
	};

	class WayPointAStar{
	public:
		// 初期化
		void Init(WayPoints& wayPoints);

		// 終了処理
		void UnInit();

		// ウェイポイントの設定
		void SetWeyPoints(WayPoints& wayPoints);

		// 目指すべき座標の取得
		Vector3D GetNextPosition(const Vector3D& start, const Vector3D& goal, float range = 1.0f);

		// 進行状況の初期化
		void Reset();

		// 到着したか
		bool IsGoal()const
		{
			return bIsGoal_;
		}

	private:
		// ウェイポイントリスト
		WayPoints* pWayPoints_ = nullptr;

		// 現在のID
		int currentID_ = 0;

		// 次に目指すID
		int nextID_ = 0;

		// ゴールのID
		int goalID_ = 0;

		// 到着したか
		bool bIsGoal_ = false;

		// 距離コストの取得
		float GetDistance(const Vector3D& position, WayPoint* node);
	};
};

#endif	// #ifndef INCLUDE_IDEA_WAYPOINTASTAR_H