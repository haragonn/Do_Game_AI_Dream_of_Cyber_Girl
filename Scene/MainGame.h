#pragma once

#include "../WinMain.h"

// �����X�e�[�W�ŗV�ԃV�[��
class MainGame : public idea::Scene{
public:
	MainGame();
	~MainGame();

	// ������
	void Init()override;

	// �I������
	void UnInit()override;

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
