#pragma once

#include "../WinMain.h"

// �}�b�v�����V�[��
class EditMode : public idea::Scene{
public:
	EditMode();
	~EditMode();

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

	// �I�u�W�F�N�g�̐ݒu
	void SetObject(idea::Camera& camera);

	// �u���b�N�̐ݒu
	bool SetBlock(int& i, int& j, int count);

	// ��̐ݒu
	bool SetSlope(int& i, int& j, int count);

	// �v���C���[�̐ݒu
	void SetPlayer(int& i, int& j, int count);

	// AI�̐ݒu
	void SetVita(int& i, int& j, int count);

	// �u���b�N�̏���
	bool RemoveBlock(idea::Camera& camera);

	// �}�b�v���̍X�V
	void UpdateMapData();

	// �}�b�v���̏����o��
	void WriteMapData();
};