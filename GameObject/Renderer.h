#pragma once

#include "../WinMain.h"

// �Q�[���p�`��N���X
class Renderer : public idea::GameObject{
public:
	Renderer();
	~Renderer();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �`��
	void Draw()override;

	// �J�����̎擾
	idea::WrapCamera& GetWrapCamera();

	// �e�`��p�J�����̎擾
	idea::ShadowCamera& GetShadowCamera();

	// ��ʑ̂̒ǉ�
	void AddObject(idea::Object& object);

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};