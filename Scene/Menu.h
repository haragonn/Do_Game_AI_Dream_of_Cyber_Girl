#pragma once

#include "../WinMain.h"

// ���[�h�I����ʂ̃V�[��
class Menu : public idea::Scene{
public:
	Menu();
	~Menu();

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
	Impl* pImpl_;
};