#pragma once

#include "../WinMain.h"

// �}�b�v����уI�u�W�F�N�g�𐶐�����N���X
class Map : public idea::GameObject{
public:
	Map();
	~Map();

	// ������
	void Init()override;

	// �I������
	void UnInit()override;

	// �X�V
	void Update()override;

	// �E�F�C�|�C���g���X�g�̎擾
	idea::WayPoints& GetWayPoints();

	// �}�b�v�̍s�̎擾
	int GetRow();

	// �}�b�v�̗�̎擾
	int GetCol();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;

	// ���̐���
	void CreatePlane();

	// �X�J�C�h�[���̐���
	void CreateSkyDome();

	// �u���b�N�̐���
	void CreateBlock();

	// �m�[�h�̐ڑ�
	void ConnectNode();

	// �ǂ̐���
	void CreateWall();
};