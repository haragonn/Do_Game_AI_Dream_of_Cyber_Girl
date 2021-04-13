#pragma once

#include "../WinMain.h"

// �u���b�N�̎��
enum class BlockStyle{
	Box,		// ��
	UpperRight,	// �E�オ��̍�
	UpperLeft,	// ���オ��̍�
	UpperFront,	// ��O�オ��̍�
	UpperBack,	// ��������̍�
	Wall1,		// �ǁE����
	Wall2,		// �ǁE����
};

// �}�b�v�ɔz�u����u���b�N�̃N���X
class Block : public idea::GameObject{
public:
	Block();
	~Block();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �u���b�N�̎�ނ̐ݒ�
	void SetStyle(BlockStyle style);

	// �u���b�N�̎�ނ̎擾
	BlockStyle GetStyle();

	// ���|���S���̎擾
	idea::Cube& GetCube();

	// �R���W�����̎擾
	idea::BoxCollider GetCollider();

	// �F�̐ݒ�
	void SetColor(idea::Color color);

	// �Z���̍��W�̐ݒ�
	void SetCell(int x, int y);

	// �Z���̍��W�̎擾
	idea::Cell GetCell();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};