#pragma once

#include "../WinMain.h"

// �A�C�e���N���X
class Item : public idea::GameObject{
public:
	Item();
	~Item();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �Փˎ�����
	void HitObstacle(const idea::Vector3D& axis);

	// �Z���̐ݒ�
	void SetCell(int x, int y);

	// �Z���̎擾
	idea::Cell& GetCell();

	// �R���W�����̎擾
	idea::SphereCollider GetCollider();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};