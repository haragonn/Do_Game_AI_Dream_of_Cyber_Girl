#include "../../h/Framework/GameManager.h"
#include "../../h/Framework/SequenceManager.h"

using namespace idea;

Scene* GameManager::GetScenePtr()
{
	return SequenceManager::Instance().GetScenePtr();
}

void GameManager::SetNextScenePtr(Scene* pNextScene)
{
	return SequenceManager::Instance().SetNextScenePtr(pNextScene);
}
