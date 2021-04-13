#ifndef INCLUDE_IDEA_BEHAVIOR_TREE_H
#define INCLUDE_IDEA_BEHAVIOR_TREE_H

#include <vector>

namespace idea{
	// 実行状態
	enum class BEHAVIOR_STATE{
		FAILED,		// 実行失敗
		RUNNING,		// 実行中
		COMPLETE	// 実行成功
	};

	// ビヘイビアツリーのタスククラス
	class BehaviorTask{
	public:
		// 実行
		virtual BEHAVIOR_STATE Run() = 0;
	};

	// ビヘイビアツリーのルートクラス
	class RootNode : public BehaviorTask{
	public:
		RootNode() :
			pChild_(nullptr)
		{
		}

		// ノードの設定
		bool SetNode(BehaviorTask& node);

		// 実行
		BEHAVIOR_STATE Run()override;

	private:
		// 子ノードのリスト
		BehaviorTask* pChild_ = nullptr;
	};

	// ビヘイビアツリーのノードクラス
	class BehaviorNode : public BehaviorTask{
	public:
		// ノードの追加
		virtual bool AddNode(BehaviorTask& node);

		// 実行
		virtual BEHAVIOR_STATE Run() = 0;

	protected:
		// 子ノードのリスト
		std::vector<BehaviorTask*> pChildren_;
	};

	// ビヘイビアツリーのセレクタークラス
	class SelectorNode : public BehaviorNode{
	public:
		SelectorNode() :
			pRunningNode_(nullptr)
		{
		}

		// 実行
		BEHAVIOR_STATE Run()override;

	private:
		BehaviorTask* pRunningNode_ = nullptr;
	};

	// ビヘイビアツリーのシーケンスクラス
	class SequenceNode : public BehaviorNode{
	public:
		SequenceNode() :
			pRunningNode_(nullptr)
		{
		}

		// 実行
		BEHAVIOR_STATE Run()override;

	private:
		BehaviorTask* pRunningNode_ = nullptr;
	};
}

#endif	// #ifndef INCLUDE_IDEA_BEHAVIOR_TREE_H