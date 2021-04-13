#pragma once

#include "../WinMain.h"

// vita�̏��
enum class VitaState{
	Idol,			// �ҋ@
	StopBy,			// �߂Â�
	Chase,			// �ǂ�������
	Escape,			// ������
	Withdrawal,		// ���E
	Count,			// �J�E���g
	Appearance,		// �o��
	Disappearance	// ����
};

// vita�̖���
enum class VitaRole{
	Main,		// �{��
	Chase,		// �ǂ�������
	Wrap,		// ����
	Ambush		// �҂�����
};

// AI�L�����N�^�[�̃N���X(�\�[�X���ł̓��f���̖��O����Ƃ���vita�ƌď̂���)
class Vita : public idea::StateMachine<VitaState>{
public:
	Vita();
	~Vita();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// ��Q���ɍ������ۂ̃��A�N�V����
	void HitObstacle(const idea::Vector3D& axis, const idea::Vector3D& back);

	// �Z���̐ݒ�
	void SetCell(int x, int y);

	// �Z���̎擾
	idea::Cell& GetCell();

	// �R���C�_�[�̎擾
	idea::CapsuleCollider GetCollider();

	// �����̐ݒ�
	void SetRole(VitaRole role);

	// �����̐ݒ�
	VitaRole GetRole();

	// �ǂ��������Ԃֈڍs
	void Chase();

	// �ҋ@��Ԃֈڍs
	void Idol();

	// �������Ԃֈڍs
	void Escape();

	// �J�E���g��Ԃֈڍs
	void Count();

	// �o���Ԃֈڍs
	void Appearance();

	// ���ŏ�Ԃֈڍs
	void Disappearance();

	// ������ڕW���擾
	idea::Vector3D GetEscapePoint();

	// �ǂ������鑬�x�̒���
	void SetChaseSpeedRate(float rate);

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};