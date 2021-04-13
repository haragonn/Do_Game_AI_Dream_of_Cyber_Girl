#pragma once

#include "../WinMain.h"

namespace{
	// ��ԕω�����
	static const int STATE_TIME = 180;
}

// �L�����N�^�[�̏��
enum class TitleCharacterState{
	Idol,	// �ҋ@
	Chase,	// �ǂ�������
	Escape	// ������
};

// �^�C�g���ŕ\�������L�����N�^�[�B���Ǘ�����N���X
class TitleCharacter : public idea::StateMachine<TitleCharacterState>{
public:
	TitleCharacter();
	~TitleCharacter();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};