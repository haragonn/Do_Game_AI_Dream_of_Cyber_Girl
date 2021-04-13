#pragma once

#include "../WinMain.h"

// �G�f�B�b�g���[�h�p�̃����_���[�N���X
class EditRenderer : public idea::GameObject{
public:
	EditRenderer();
	~EditRenderer();

	// ������
	void Init();

	// �X�V
	void Update();

	// �`��
	void Draw();

	// �J�����̎擾
	idea::WrapCamera& GetWrapCamera();

	// ��ʑ̂̒ǉ�
	void AddObject(idea::Object& object);

private:
	// Pimpl�C�f�B�I��
	class impl;

	// ����
	impl* pImpl_ = nullptr;
};