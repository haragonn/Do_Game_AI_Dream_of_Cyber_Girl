#pragma once

#include "../WinMain.h"

// �^�C�g����ʂ̃V�[��
class Title : public idea::Scene{
public:
	Title();
	~Title();

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