#pragma once

#include "../WinMain.h"

namespace{
	// �Q�[������
	static const int TIME_UP = 60 * 60;

	// ���ҕs��
	static const int ABSENCE = 0;

	// �v���C���[����
	static const int PLAYER_WIN = 1;

	// vita����
	static const int VITA_WIN = 2;
}

// ���s����N���X
class Judg : public idea::GameObject{
public:
	Judg();
	~Judg();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �v���C���[�������Ԃ̎擾
	int GetPlayerTime();

	// vita�������Ԃ̎擾
	int GetVitaTime();

	// ���҂̎擾
	int GetWinner();

	// �v���C���[���I�j�Ȃ����u�Ԃ��Z�b�g����
	void SetPlayerOniFlag(bool flag);

	// �v���C���[�̓I�j��
	bool IsPlayerOni();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};