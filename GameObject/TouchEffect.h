#pragma once

#include "../WinMain.h"

// �^�b�`�����ۂ̃G�t�F�N�g�̃N���X
class TouchEffect : public idea::GameObject{
public:
	TouchEffect();
	~TouchEffect();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �F�̐ݒ�
	void SetColor(idea::Color color);

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};