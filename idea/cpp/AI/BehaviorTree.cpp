#include "../../h/AI/BehaviorTree.h"

using namespace idea;

bool RootNode::SetNode(BehaviorTask& node)
{
	if(pChild_){
		return false;
	}

	pChild_ = &node;

	return true;
}

BEHAVIOR_STATE RootNode::Run()
{
	if(!pChild_){
		return BEHAVIOR_STATE::FAILED;
	}

	return pChild_->Run();
}

bool BehaviorNode::AddNode(BehaviorTask& node)
{
	if(std::find(pChildren_.begin(), pChildren_.end(), &node) == pChildren_.end()){
		pChildren_.push_back(&node);

		return true;
	}

	return false;
}

BEHAVIOR_STATE BehaviorNode::Run()
{
	if(!pChildren_.size()){
		return BEHAVIOR_STATE::FAILED;
	}

	BEHAVIOR_STATE state = BEHAVIOR_STATE::FAILED;

	for(auto& node : pChildren_){
		state = node->Run();

		if(state == BEHAVIOR_STATE::FAILED){
			return state;
		}
	}

	return state;
}

BEHAVIOR_STATE SelectorNode::Run()
{
	if(!pChildren_.size()){
		return BEHAVIOR_STATE::FAILED;
	}

	BEHAVIOR_STATE state = BEHAVIOR_STATE::FAILED;

	bool isRunningFailed = false;

	if(pRunningNode_){
		state = pRunningNode_->Run();

		if(state == BEHAVIOR_STATE::COMPLETE){
			pRunningNode_ = nullptr;

			return state;
		} else if(state == BEHAVIOR_STATE::RUNNING){
			return state;
		} else if(state == BEHAVIOR_STATE::FAILED){
			isRunningFailed = true;
		}
	}

	for(auto& node : pChildren_){
		if(isRunningFailed){
			if(node != pRunningNode_){
				continue;
			}

			isRunningFailed = false;

			pRunningNode_ = nullptr;

			continue;
		}

		state = node->Run();

		if(state == BEHAVIOR_STATE::COMPLETE){
			return state;
		} else if(state == BEHAVIOR_STATE::RUNNING){
			pRunningNode_ = node;

			return state;
		}
	}

	return state;
}

BEHAVIOR_STATE SequenceNode::Run()
{
	if(!pChildren_.size()){
		return BEHAVIOR_STATE::FAILED;
	}

	BEHAVIOR_STATE state = BEHAVIOR_STATE::FAILED;

	bool isRunningComplete = false;

	if(pRunningNode_){
		state = pRunningNode_->Run();

		if(state == BEHAVIOR_STATE::FAILED){
			pRunningNode_ = nullptr;

			return state;
		} else if(state == BEHAVIOR_STATE::RUNNING){
			return state;
		} else if(state == BEHAVIOR_STATE::COMPLETE){
			isRunningComplete = true;
		}
	}

	for(auto& node : pChildren_){
		if(isRunningComplete){
			if(node != pRunningNode_){
				continue;
			}

			isRunningComplete = false;

			pRunningNode_ = nullptr;

			continue;
		}

		state = node->Run();

		if(state == BEHAVIOR_STATE::FAILED){
			return state;
		} else if(state == BEHAVIOR_STATE::RUNNING){
			pRunningNode_ = node;

			return state;
		}
	}

	return state;
}