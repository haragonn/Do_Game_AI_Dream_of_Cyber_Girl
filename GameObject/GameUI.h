#pragma once

#include "../WinMain.h"

// �Q�[����ʂ�UI���Ǘ�����N���X
class GameUI : public idea::GameObject{
public:
	GameUI();
	~GameUI();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �`��
	void Draw()override;

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};
