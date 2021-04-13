#ifndef INCLUDE_IDEA_BEHAVIOR_TREE_H
#define INCLUDE_IDEA_BEHAVIOR_TREE_H

#include <vector>

namespace idea{
	// ���s���
	enum class BEHAVIOR_STATE{
		FAILED,		// ���s���s
		RUNNING,		// ���s��
		COMPLETE	// ���s����
	};

	// �r�w�C�r�A�c���[�̃^�X�N�N���X
	class BehaviorTask{
	public:
		// ���s
		virtual BEHAVIOR_STATE Run() = 0;
	};

	// �r�w�C�r�A�c���[�̃��[�g�N���X
	class RootNode : public BehaviorTask{
	public:
		RootNode() :
			pChild_(nullptr)
		{
		}

		// �m�[�h�̐ݒ�
		bool SetNode(BehaviorTask& node);

		// ���s
		BEHAVIOR_STATE Run()override;

	private:
		// �q�m�[�h�̃��X�g
		BehaviorTask* pChild_ = nullptr;
	};

	// �r�w�C�r�A�c���[�̃m�[�h�N���X
	class BehaviorNode : public BehaviorTask{
	public:
		// �m�[�h�̒ǉ�
		virtual bool AddNode(BehaviorTask& node);

		// ���s
		virtual BEHAVIOR_STATE Run() = 0;

	protected:
		// �q�m�[�h�̃��X�g
		std::vector<BehaviorTask*> pChildren_;
	};

	// �r�w�C�r�A�c���[�̃Z���N�^�[�N���X
	class SelectorNode : public BehaviorNode{
	public:
		SelectorNode() :
			pRunningNode_(nullptr)
		{
		}

		// ���s
		BEHAVIOR_STATE Run()override;

	private:
		BehaviorTask* pRunningNode_ = nullptr;
	};

	// �r�w�C�r�A�c���[�̃V�[�P���X�N���X
	class SequenceNode : public BehaviorNode{
	public:
		SequenceNode() :
			pRunningNode_(nullptr)
		{
		}

		// ���s
		BEHAVIOR_STATE Run()override;

	private:
		BehaviorTask* pRunningNode_ = nullptr;
	};
}

#endif	// #ifndef INCLUDE_IDEA_BEHAVIOR_TREE_H