#pragma once

#include "../WinMain.h"

// �}�E�X�|�C���^�ɔ�������p�l���N���X
class Panel : public idea::GameObject{
public:
	Panel();
	~Panel();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �Z���̐ݒ�
	void SetCell(int x, int y);

	// �F�̐ݒ�
	void SetColor(idea::Color color);

	// ���̎擾
	idea::Cube& GetCube();

	// �R���C�_�[�̎擾
	idea::BoxCollider GetCollider();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};