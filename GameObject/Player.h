#pragma once

#include "../WinMain.h"

// �v���C���[�̏��
enum class PlayerState{
	Idol,	// �ҋ@
	Walk,	// �ړ�
	Back,	// ������΂����
	Count	// �J�E���g
};

// �v���C���[�L�����N�^�[�̃N���X
class Player : public idea::StateMachine<PlayerState>{
public:
	Player();
	~Player();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �J�E���g��Ԃֈڍs
	void Count();

	// ��Q���ɍ������ۂ̃��A�N�V����
	void HitObstacle(const idea::Vector3D& axis, const idea::Vector3D& back);

	// �A�C�e���̎擾���̃��A�N�V����
	void Doping();

	// �Z���̐ݒ�
	void SetCell(int x, int y);

	// �Z���̎擾
	idea::Cell& GetCell();

	// �R���C�_�[�̎擾
	idea::CapsuleCollider GetCollider();

	// �X�^�~�i�c�ʂ̎擾
	float GetStamina();

	// ��J��Ԏ��Ԃ̎擾
	int GetExhausted();

	// �A�C�e��������Ԏ��Ԃ̎擾
	int GetDoping();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;

	// ���ڂ���vita�̌���
	void LookOnVita();
};