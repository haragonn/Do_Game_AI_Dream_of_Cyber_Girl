#pragma once

#include "../WinMain.h"

// �^�C�g����UI���Ǘ�����N���X
class TitleUI : public idea::GameObject{
public:
	TitleUI();
	~TitleUI();

	// ������
	void Init()override;

	// �X�V
	void Update()override;

	// �`��
	void Draw()override;

private:
	// Pimpl�C�f�B�I��
	class Impl;

	// ����
	Impl* pImpl_ = nullptr;
};