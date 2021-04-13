#include <algorithm>
#include <numeric>

#include "../../h/AI/WayPointAStar.h"
#include "../../h/Utility/CsvReader.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;
using namespace idea;

WayPoint::WayPoint()
{
	id_ = -1;

	cost_ = 0.0f;

	Sphere::Init(Vector3DZero());

	state_ = CheckState::NO_CHECK;
}

WayPoint::WayPoint(int id, Vector3D position, float cost)
{
	id_ = id;

	cost_ = cost;

	Sphere::Init(position);

	state_ = CheckState::NO_CHECK;
}

void WayPoint::AddNode(WayPoint* node)
{
	if(node){
		if(find(nodes_.begin(), nodes_.end(), node) == nodes_.end()){
			nodes_.push_back(node);
		}
	}
}

void WayPoint::RemoveNode(WayPoint* node)
{
	if(node){
		auto n = find(nodes_.begin(), nodes_.end(), node);

		if(n != nodes_.end()){
			nodes_.erase(n);
		}
	}
}

vector<WayPoint*> WayPoint::GetNodes()
{
	return nodes_;
}

WayPoints::~WayPoints()
{
	UnInit();
}

void WayPoints::Init(Camera* pCamera)
{
	pCamera_ = pCamera;
}

void WayPoints::UnInit()
{
	for(auto& wayPoint : wayPoints_){
		SafeDelete(wayPoint);
	}

	vector<WayPoint*>().swap(wayPoints_);

	bAnalyze_ = false;

	pCamera_ = nullptr;
}

bool WayPoints::LoadWayPoint(const char* pFileName)
{
	CsvReader cr;

	if(!cr.LoadData(pFileName)){
		return false;
	}

	for(int i = 0; i < cr.GetRow(); ++i){
		Vector3D pos = Vector3D(cr[i]["posx"], cr[i]["posy"], cr[i]["posz"]);

		AddWayPoint(pos);
	}

	for(int i = 0; i < cr.GetRow(); ++i){
		for(int j = 3; j < cr.GetCol(); ++j){
			if((int)cr[i][j] != -1){
				wayPoints_[i]->AddNode(GetWayPoint((int)cr[i][j]));
			}
		}
	}

	return true;
}

bool WayPoints::LoadGraph(const char* pFileName)
{
	CsvReader cr;

	if(!cr.LoadData(pFileName)){
		return false;
	}

	for(int i = 0; i < cr.GetRow(); ++i){
		for(int j = 0; j < cr.GetCol(); ++j){
				graph_[i][j] = (int)cr[i][j];
		}
	}

	bAnalyze_ = true;

	return true;
}

bool WayPoints::WriteWayPoint(const char* pFileName)
{
	if(wayPoints_.size()){
		vector<const char*> id;
		vector<const char*> key;

		// ポジションの書き込み
		DynamicDualArray<float> pos;

		for(int i = 0; i < (int)key.size(); ++i){
			pos[i][0] = wayPoints_[i]->GetPosition().x;
			pos[i][1] = wayPoints_[i]->GetPosition().y;
			pos[i][2] = wayPoints_[i]->GetPosition().z;
		}

		// ノードの書き込み
		DynamicDualArray<int> nodeData;

		for(int i = 0; i < (int)wayPoints_.size(); ++i){
			auto node = wayPoints_[i]->GetNodes();

			sort(node.begin(), node.end(),
				[](WayPoint* lhs, WayPoint* rhs) {
					return lhs->GetID() < rhs->GetID();
				});

			for(int j = 0; j < (int)wayPoints_.size(); ++j){
				if(j < (int)wayPoints_[i]->GetNodes().size()){
					nodeData[i][j] = node[j]->GetID();
				} else{
					nodeData[i][j] = -1;
				}
			}
		}

		id.push_back("posx");
		id.push_back("posy");
		id.push_back("posz");

		for(int i = 0; i < (int)nodeData[0].GetSize(); ++i){
			ostringstream oss;
			char* c = new char[256];

			sprintf_s(c, 255, "node%02d", i);

			id.push_back(c);
		}

		for(auto& wayPoint : wayPoints_){
			ostringstream oss;
			char* c = new char[256];

			int num = wayPoint->GetID();

			sprintf_s(c, 255, "point%02d", num);

			key.push_back(c);
		}

		DynamicDualArray<string> data;

		for(int i = 0; i < (int)key.size(); ++i){
			ostringstream ossx;
			ossx << wayPoints_[i]->GetPosition().x;
			data[i][0] = ossx.str();
			ostringstream ossy;
			ossy << wayPoints_[i]->GetPosition().y;
			data[i][1] = ossy.str();
			ostringstream ossz;
			ossz << wayPoints_[i]->GetPosition().z;
			data[i][2] = ossz.str();
		}

		for(int i = 0; i < (int)key.size(); ++i){
			for(int j = 3; j < (int)nodeData[i].GetSize() + 3; ++j){
				ostringstream oss;
				oss << nodeData[i][j - 3];
				data[i][j] = oss.str();
			}
		}

		if(!CsvWriter::WriteData<string>(pFileName, data, id, key)){
			for(int i = 3; i < (int)nodeData[0].GetSize() + 3; ++i){
				SafeDeleteArray(id[i]);
			}

			for(auto k : key){
				SafeDeleteArray(k);
			}

			return false;
		}

		for(int i = 3; i < (int)nodeData[0].GetSize() + 3; ++i){
			SafeDeleteArray(id[i]);
		}

		for(auto k : key){
			SafeDeleteArray(k);
		}

		return true;
	}

	return false;
}

bool WayPoints::WriteGraph(const char* pFileName)
{
	if(!graph_.GetSize()){
		return false;
	}

	vector<const char*> id;
	vector<const char*> key;

	for(int i = 0; i < (int)graph_[0].GetSize(); ++i){
		ostringstream oss;
		char* c = new char[256];

		sprintf_s(c, 255, "node%02d", i);

		id.push_back(c);
	}

	for(int i = 0; i < (int)graph_[0].GetSize(); ++i){
		ostringstream oss;
		char* c = new char[256];

		sprintf_s(c, 255, "node%02d", i);

		key.push_back(c);
	}

	if(!CsvWriter::WriteData<int>(pFileName, graph_, id, key)){
		for(auto i : id){
			SafeDeleteArray(i);
		}

		for(auto k : key){
			SafeDeleteArray(k);
		}

		return false;
	}

	for(auto i : id){
		SafeDeleteArray(i);
	}

	for(auto k : key){
		SafeDeleteArray(k);
	}

	return true;
}

int WayPoints::AddWayPoint(const Vector3D& position, float cost)
{
	int n = wayPoints_.size();

	for(int i = 0; i < n; ++i){
		auto it = find_if(wayPoints_.begin(), wayPoints_.end(),
			[i](const WayPoint* p)
			{
				return p->id_ == i;
			});

		if(it == wayPoints_.end()){
			auto point = new WayPoint(i, position, cost);

			wayPoints_.push_back(point);

			if(pCamera_){
				pCamera_->AddObject((*point));
			}

			return i;
		}
	}

	auto point = new WayPoint(n, position, cost);

	wayPoints_.push_back(point);

	if(pCamera_){
		pCamera_->AddObject((*point));
	}

	return n;
}

void WayPoints::RemoveWayPoint(int id)
{
	auto n = find_if(wayPoints_.begin(), wayPoints_.end(),
		[id](const WayPoint* p)
		{
			return p->id_ == id;
		});

	if(n != wayPoints_.end()){
		wayPoints_.erase(n);
	}
}

void WayPoints::ConnectWayPoint(int id1, int id2)
{
	if(id1 < 0 || (int)wayPoints_.size() <= id1){
		return;
	}

	if(id2 < 0 || (int)wayPoints_.size() <= id2){
		return;
	}

	auto p1 = GetWayPoint(id1);
	auto p2 = GetWayPoint(id2);

	if(p1 && p2){
		p1->AddNode(p2);
		p2->AddNode(p1);
	}
}

void WayPoints::ConnectWayPoint(WayPoint* pPoint1, WayPoint* pPoint2)
{

	if(pPoint1 && pPoint2){
		pPoint1->AddNode(pPoint2);
		pPoint2->AddNode(pPoint1);
	}
}

void WayPoints::DisconnectWayPoint(int id1, int id2)
{
	if(id1 < 0 || (int)wayPoints_.size() <= id1){
		return;
	}

	if(id2 < 0 || (int)wayPoints_.size() <= id2){
		return;
	}

	auto p1 = GetWayPoint(id1);
	auto p2 = GetWayPoint(id2);

	if(p1 && p2){
		p1->RemoveNode(p2);
		p2->RemoveNode(p1);
	}
}

void WayPoints::DisconnectWayPoint(WayPoint* pPoint1, WayPoint* pPoint2)
{
	if(pPoint1 && pPoint2){
		pPoint1->RemoveNode(pPoint2);
		pPoint2->RemoveNode(pPoint1);
	}
}

int WayPoints::GetWayPointSize() const
{
	return static_cast<int>(wayPoints_.size());
}

WayPoint* WayPoints::GetWayPoint(int id)
{
	auto n = find_if(wayPoints_.begin(), wayPoints_.end(),
		[id](const WayPoint* p)
		{
			return p->id_ == id;
		});

	if(n != wayPoints_.end()){
		return (*n);
	}

	return nullptr;
}

vector<WayPoint*>& WayPoints::GetWayPoints()
{
	return wayPoints_;
}

DynamicDualArray<int>& WayPoints::GetGraph()
{
	return graph_;
}

bool WayPoints::Analyze()
{
	if(bAnalyze_){
		return true;
	}

	for(int i = 0; i < (int)wayPoints_.size(); ++i){
		for(int j = 0; j < (int)wayPoints_.size() * (int)wayPoints_.size(); ++j){
			route_[j][i] = -1;
		}
	}

	for(auto& node : wayPoints_){
		row_ = 0;
		for(auto& node2 : wayPoints_){
			if(node == node2){
				continue;
			}

			ResetSearchStatus();
			col_ = 0;
			node->state_ = WayPoint::CheckState::OPEN;

			bool isConnect = Analyze(node, node2);

			if(isConnect){
				vector<int>().swap(path_);
				TraceRoute(node, node2);
				path_.push_back(node->id_);
				reverse(path_.begin(), path_.end());
				for(int i = 0; i < (int)path_.size(); ++i){
					route_[node->id_ * wayPoints_.size() + row_][col_] = path_[i];
					++col_;
				}
				++row_;
			} else{
				for(int i = 0; i < (int)path_.size(); ++i){
					route_[node->id_ * wayPoints_.size() + row_][col_] = -1;
					++col_;
				}
				++row_;
			}
		}
	}

	bAnalyze_ = true;

	Graphing();

	return bAnalyze_;
}

void WayPoints::ColorReset()
{
	if(bVisible_){
		for(auto& point : wayPoints_){
			point->SetColor(ideaColor::WHITE);
		}
	}else {
		for(auto& point : wayPoints_){
			point->SetColor(ideaColor::CLEAR);
		}
	}
}

void WayPoints::SetVisibleFlag(bool bFlag)
{
	bVisible_ = bFlag;

	if(!bVisible_){
		for(auto& point : wayPoints_){
			point->SetColor(ideaColor::CLEAR);
		}
	}
}

bool WayPoints::Analyze(WayPoint* node, WayPoint* node2)
{
	float costMax = FLT_MAX;

	WayPoint* n = nullptr;

	// コストが最小のオープンノードを探す
	for(auto& p : wayPoints_){
		if(p->state_ != WayPoint::CheckState::OPEN){
			continue;
		}

		// ヒューリックコストの測定
		float cost = BackTrace(p, node, 0) * 10000.0f + GetDistance(p, node2) + p->cost_;

		if(cost > costMax){
			continue;
		}

		costMax = cost;

		n = p;
	}

	// オープンノードがなければ終了(八方ふさがり)
	if(!n){
		node->parent_ = nullptr;
		return false;
	}

	// ノードをクローズする
	n->state_ = WayPoint::CheckState::CLOSE;

	// ゴールに着いたら探索終了
	if(n == node2){
		n->state_ = WayPoint::CheckState::CLOSE;
		return true;
	}

	for(auto& p : n->nodes_){
		if(p->state_ == WayPoint::CheckState::NO_CHECK){
			p->parent_ = n;

			p->state_ = WayPoint::CheckState::OPEN;
		}
	}

	return Analyze(node, node2);
}

void WayPoints::ResetSearchStatus()
{
	col_ = 0;

	for(auto& node : wayPoints_){
		node->state_ = WayPoint::CheckState::NO_CHECK;
	}
}

int WayPoints::BackTrace(WayPoint* node, WayPoint* node2, int count)
{
	if(count >= 10000){
		return 10000;
	}

	// スタートに着いたら探索終了
	if(node == node2){
		return 1;
	}

	return BackTrace(node->parent_,
		node2,
		count + 1
	) + 1;
}

float WayPoints::GetDistance(WayPoint* node, WayPoint* node2)
{
	return (node->GetPosition() - node2->GetPosition()).LengthSquare();
}

void WayPoints::TraceRoute(WayPoint* node, WayPoint* node2)
{
	if(node == node2){
		return;
	}

	path_.push_back(node2->id_);

	if(!node2->parent_){
		return;
	}

	TraceRoute(
		node,
		node2->parent_);
}

void WayPoints::Graphing()
{
	if(!bAnalyze_){
		return;
	}

	graph_.Reserve((int)wayPoints_.size(), (int)wayPoints_.size(), -1);

	int n = 0;

	for(int i = 0; i < (int)wayPoints_.size(); ++i){	// ID
		n = 0;
		for(int j = 0; j < (int)wayPoints_.size(); ++j){	// 目的地
			if(i != j){
				graph_[i][j] = route_[i * wayPoints_.size() + n][1];
				++n;
			}
		}
	}

}

void WayPointAStar::Init(WayPoints& wayPoints)
{
	SetWeyPoints(wayPoints);
}

void WayPointAStar::UnInit()
{
	pWayPoints_ = nullptr;
}

void WayPointAStar::SetWeyPoints(WayPoints& wayPoints)
{
	pWayPoints_ = &wayPoints;

	currentID_ = -1;

	nextID_ = -1;

	goalID_ = -1;
}

Vector3D WayPointAStar::GetNextPosition(const Vector3D& start, const Vector3D& goal, float range)
{
	if(!pWayPoints_){
		return start;
	}

	if(!pWayPoints_->IsAnalyze()){
		pWayPoints_->Analyze();
		return start;
	}

	auto& wayPoints = pWayPoints_->GetWayPoints();

	auto& graph = pWayPoints_->GetGraph();

	auto bVisible = pWayPoints_->IsVisible();

	{
		float costMax = FLT_MAX;
		WayPoint* n = nullptr;

		for(auto& p : wayPoints){
			float cost = GetDistance(goal, p);

			if(cost > costMax){
				continue;
			}

			costMax = cost;

			n = p;
		}

		if(n){
			goalID_ = n->GetID();

			if(bVisible){
				n->SetColor(ideaColor::SOFT_RED);
			}
		} else{
			return start;
		}
	}

	if(currentID_ == -1 || nextID_ == -1){
		bIsGoal_ = false;

		float costMax = FLT_MAX;
		WayPoint* n = nullptr;

		for(auto& p : wayPoints){
			float cost = GetDistance(start, p);

			if(cost > costMax){
				continue;
			}

			costMax = cost;

			n = p;
		}

		if(n){
			currentID_ = n->GetID();

			if(bVisible){
				n->SetColor(ideaColor::CYAN);
			}

			if(!bIsGoal_ && goalID_ == currentID_){
				int id = goalID_;

				currentID_ = -1;
				goalID_ = -1;

				bIsGoal_ = true;

				auto n = pWayPoints_->GetWayPoint(id);

				if(n){
					return n->GetPosition();
				}
			}

			nextID_ = graph[currentID_][goalID_];

			if(nextID_ == -1){
				return start;
			}
		} else{
			return start;
		}
	} else{
		int id = nextID_;

		auto n = pWayPoints_->GetWayPoint(id);

		Vector3D pos;

		if(n){
			pos = n->GetPosition();

			if(bVisible){
				n->SetColor(ideaColor::CYAN);
			}
		} else{
			return start;
		}


		if((start - pos).ReplaceY(0.0f).Length() < range){
			currentID_ = nextID_;

			if(!bIsGoal_ && goalID_ == currentID_){
				int id = goalID_;

				currentID_ = -1;
				goalID_ = -1;

				bIsGoal_ = true;

				auto n = pWayPoints_->GetWayPoint(id);

				if(n){
					return n->GetPosition();
				}
			}

			nextID_ = graph[currentID_][goalID_];

			if(nextID_ == -1){
				return start;
			}
		}
	}

	if(!bIsGoal_ && goalID_ == currentID_){
		int id = goalID_;

		currentID_ = -1;
		goalID_ = -1;

		bIsGoal_ = true;

		auto n = pWayPoints_->GetWayPoint(id);

		if(n){
			return n->GetPosition();
		}
	}

	{
		int id = nextID_;

		auto n = pWayPoints_->GetWayPoint(id);

		if(n){
			return n->GetPosition();
		}
	}

	return start;
}

void WayPointAStar::Reset()
{
	currentID_ = -1;

	nextID_ = -1;

	goalID_ = -1;
}

float WayPointAStar::GetDistance(const Vector3D& position, WayPoint* node)
{
	return (position - node->GetPosition()).LengthSquare();
}
