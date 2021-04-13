#pragma once

#include "../WinMain.h"

// �^�C�g���p�`��N���X
class TitleRenderer : public idea::GameObject{
public:
	TitleRenderer();
	~TitleRenderer();

	// ������
	void Init();

	// �`��
	void Draw();

	// �J�����̎擾
	idea::WrapCamera& GetWrapCamera();

	// ��ʑ̂̒ǉ�
	void AddObject(idea::Object& object);

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};