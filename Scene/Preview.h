#pragma once

#include "../WinMain.h"

// �쐬�����X�e�[�W������������V�[��
class Preview : public idea::Scene{
public:
	Preview();
	~Preview();

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