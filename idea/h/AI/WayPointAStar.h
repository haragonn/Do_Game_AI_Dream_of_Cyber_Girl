#ifndef INCLUDE_IDEA_WAYPOINTASTAR_H
#define INCLUDE_IDEA_WAYPOINTASTAR_H

#include <vector>

#include "../3D/Sphere.h"
#include "../Utility/ideaMath.h"
#include "../Utility/DynamicDualArray.h"

namespace idea{
	// ���W
	class WayPoint : public Sphere{
	public:
		WayPoint();

		WayPoint(int id, Vector3D position, float cost);

		// ID�̎擾
		int GetID()const
		{
			return id_;
		}

		// �m�[�h�̒ǉ�
		void AddNode(WayPoint* node);

		// �m�[�h�̏���
		void RemoveNode(WayPoint* node);

		// �m�[�h�̎擾
		std::vector<WayPoint*> GetNodes();

	private:
		// �`�F�b�N���
		enum class CheckState{
			NO_CHECK,
			OPEN,
			CLOSE,
		}state_;

		friend class WayPoints;

		// ID
		int id_ = 0;

		// �R�X�g
		float cost_ = 0.0f;

		// �m�[�h���X�g
		std::vector<WayPoint*> nodes_;

		// �e�m�[�h
		WayPoint* parent_ = nullptr;
	};

	class WayPoints{
	public:
		~WayPoints();

		// ������
		void Init(Camera* pCamera);

		// �I������
		void UnInit();

		// �E�F�C�|�C���g���̓ǂݍ���
		bool LoadWayPoint(const char* pFileName);

		// �O���t���̓ǂݍ���
		bool LoadGraph(const char* pFileName);

		// �E�F�C�|�C���g���̏����o��
		bool WriteWayPoint(const char* pFileName);

		// �O���t���̏����o��
		bool WriteGraph(const char* pFileName);

		// �E�F�C�|�C���g�̒ǉ�
		int AddWayPoint(const Vector3D& position, float cost = 0.0f);

		// �E�F�C�|�C���g�̏���
		void RemoveWayPoint(int id);

		// �E�F�C�|�C���g�̐ڑ�
		void ConnectWayPoint(int id1, int id2);

		// �E�F�C�|�C���g�̐ڑ�
		void ConnectWayPoint(WayPoint* pPoint1, WayPoint* pPoint2);

		// �E�F�C�|�C���g�̐ؒf
		void DisconnectWayPoint(int id1, int id2);

		// �E�F�C�|�C���g�̐ڑ�
		void DisconnectWayPoint(WayPoint* pPoint1, WayPoint* pPoint2);

		// �E�F�C�|�C���g�̐��̎擾
		int GetWayPointSize()const;

		// �E�F�C�|�C���g�̎擾
		WayPoint* GetWayPoint(int id);

		// �E�F�C�|�C���g���X�g�̎擾
		std::vector<WayPoint*>& GetWayPoints();

		// �O���t�̎擾
		DynamicDualArray<int>& GetGraph();

		// �ŒZ�o�H����
		bool Analyze();

		// �F�̏�����
		void ColorReset();

		// ���͍ς݂�
		bool IsAnalyze()const
		{
			return bAnalyze_;
		}

		// ����
		bool IsVisible()const
		{
			return bVisible_;
		}

		// ���t���O�̐ݒ�
		void SetVisibleFlag(bool bFlag);

	private:
		// �����ς݂�
		bool bAnalyze_ = false;

		bool bVisible_ = false;

		// �E�F�C�|�C���g���X�g
		std::vector<WayPoint*> wayPoints_;

		Camera* pCamera_ = nullptr;

		// �s
		int row_ = 0;

		// ��
		int col_ = 0;

		// �ŒZ�o�H
		DynamicDualArray<int> route_;

		// �o�HID
		std::vector<int> path_;

		// �ŒZ�o�H�O���t
		DynamicDualArray<int> graph_;

		bool Analyze(WayPoint* node, WayPoint* node2);

		// �������̏�����
		void ResetSearchStatus();

		// ��A�R�X�g�̎擾
		int BackTrace(WayPoint* node, WayPoint* node2, int count);

		// �����R�X�g�̎擾
		float GetDistance(WayPoint* node, WayPoint* node2);

		// ���[�g�̊���o��
		void TraceRoute(WayPoint* node, WayPoint* node2);

		// �O���t��
		void Graphing();
	};

	class WayPointAStar{
	public:
		// ������
		void Init(WayPoints& wayPoints);

		// �I������
		void UnInit();

		// �E�F�C�|�C���g�̐ݒ�
		void SetWeyPoints(WayPoints& wayPoints);

		// �ڎw���ׂ����W�̎擾
		Vector3D GetNextPosition(const Vector3D& start, const Vector3D& goal, float range = 1.0f);

		// �i�s�󋵂̏�����
		void Reset();

		// ����������
		bool IsGoal()const
		{
			return bIsGoal_;
		}

	private:
		// �E�F�C�|�C���g���X�g
		WayPoints* pWayPoints_ = nullptr;

		// ���݂�ID
		int currentID_ = 0;

		// ���ɖڎw��ID
		int nextID_ = 0;

		// �S�[����ID
		int goalID_ = 0;

		// ����������
		bool bIsGoal_ = false;

		// �����R�X�g�̎擾
		float GetDistance(const Vector3D& position, WayPoint* node);
	};
};

#endif	// #ifndef INCLUDE_IDEA_WAYPOINTASTAR_H