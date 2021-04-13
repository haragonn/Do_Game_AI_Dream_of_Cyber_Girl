#pragma once

#include "../WinMain.h"

// �Q�[�����x�����Ǘ�����N���X
class MetaAI : public idea::GameObject{
public:
	MetaAI();
	~MetaAI();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �v���C���[�X�L���̕]��
	void EvaluationPlayerSkill();

	// ��Փx����
	void AdjustmentDifficulty();

	// AI���x������
	void AdjustmentVitaLevel();

	// AI�̃Z�b�g
	void SetVita();

	// AI�̒ǉ�
	void AddVita();

	// �A�C�e���̃Z�b�g
	void SetItem();

	// �f�o�b�O���̕\��
	void ShowDebug();

	// vita�̑ҋ@�͈͂̎擾
	float GetVitaIdolRange();

	// vita�̋߂Â����x�̎擾
	float GetVitaStopBySpeed();

	// vita�̒ǂ������鑬�x�̎擾
	float GetVitaChaseSpeed();

	// vita�̓����鑬�x�̎擾
	float GetVitaEscapeSpeed();

	// vita�̍����鑬�x�̃��[�g�̎擾
	float GetVitaClimbRate();

	// �I�j���؂�ւ�������Ƃ�m�点��
	void OniChange();

	// �A�C�e�����擾���ꂽ���Ƃ�m�点��
	void GetItem();

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};