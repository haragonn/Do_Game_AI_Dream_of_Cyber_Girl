#pragma once

#include "../WinMain.h"

// �L�����N�^�[�������Ǘ�����N���X
class GameVoice : public idea::GameObject{
public:
	GameVoice();
	~GameVoice();

	// ������
	void Init();

	// �X�V
	void Update();

	// �J�n�{�C�X
	void Start();

	// �v���C���[�^�b�`
	void PlayerTouch();

	// vita�^�b�`
	void VitaTouch();

	// vita���B
	void AddVita(int vitaNum);

	// vita��ʔ���
	void FlockVita();

	// �v���C���[����
	void PlayerWin();

	// vita����
	void VitaWin();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;

	// ���ׂẴ{�C�X���~�߂�
	void StopAll();
};
