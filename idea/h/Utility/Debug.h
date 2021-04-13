/*==============================================================================
	[Debug.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_DEBUG_H
#define INCLUDE_IDEA_DEBUG_H

#include "../Utility/ideaMath.h"

#include <vector>
#include <string>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FDebug
	// �N���X�T�v�F�f�o�b�O�@�\�W
	//------------------------------------------------------------------------------
	class Debug{
	public:
		// �f�o�b�O�\������
		bool IsDebugMode();

		// �f�o�b�O�X�C�b�`���I����
		bool GetDebugSwitch(int index)const;

		// �f�o�b�O�X�C�b�`�̐؂�ւ�
		void SetDebugSwitch(int index, bool flag);

		// ���O�̏�������
		void Log(const char* pFormat, ...);

		// ���O�̃N���A
		void ClearLog();

		// �ϐ��̃g���[�X
		void Trace(const char* pVariableName, int value);

		// �ϐ��̃g���[�X
		void Trace(const char* pVariableName, float value);

		// �ϐ��̃g���[�X
		void Trace(const char* pVariableName, const Vector2D& value);

		// �ϐ��̃g���[�X
		void Trace(const char* pVariableName, const Vector3D& value);

		// �ϐ��̃g���[�X
		void Trace(const char* pVariableName, const Vector4D& value);

		// �g���[�X���̃N���A
		void ClearTrace();
	};
};

#endif	// #ifndef INCLUDE_IDEA_DEBUG_H