#pragma once

#include "../WinMain.h"

// ��������̃V�[��
class Tutorial : public idea::Scene{
public:
	Tutorial();
	~Tutorial();

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
	Impl* pImpl_;
};