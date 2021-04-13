/*==============================================================================
	[DebugManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_DEBUGMANAGER_H
#define INCLUDE_IDEA_DEBUGMANAGER_H

#include "../Input/Controller.h"
#include "../Utility/Timer.h"
#include "../Utility/ideaMath.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FDebugManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@�f�o�b�O���̊Ǘ����s��
	//------------------------------------------------------------------------------
	class DebugManager{
	public:
		// �B��̃C���X�^���X
		static DebugManager& Instance()
		{
			static DebugManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init();

		// �I������
		void UnInit();

		// �X�V
		void Update(bool bResourceDraw);

		// �f�o�b�O�\������
		bool IsDebugMode()const
		{
			return bDraw_;
		}

		// �f�o�b�O�X�C�b�`���I����
		bool GetDebugSwitch(int index)const;

		// �f�o�b�O�X�C�b�`�̐؂�ւ�
		void SetDebugSwitch(int index, bool flag);

		// ���O�̏�������
		void Log(std::string str);

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

		// ���\�[�X���̒ǉ�
		void AddResource(const char* pFileName);

		// ���\�[�X���̔j��
		void RemoveResource(const char* pFileName);

		// �^�C�}�[�̏�����
		void RefreshTime();

	private:
		// �R���g���[���[
		Controller controller_;

		// �^�C�}�[
		Timer tmr_;

		// �`��t���O
		bool bDraw_ = false;

		// �f�o�b�O�X�C�b�`
		bool bDebugSwitch_[3] = {};

		// ���O�`��t���O
		bool bShowLog_ = false;

		// int�ϐ��`��t���O
		bool bShowTrace_ = false;

		// ���\�[�X���`��t���O
		bool bShowResource_ = false;

		// �݌v�t���[��
		unsigned int frame_ = 0U;

		// �X�V���Ԃ̒~�σf�[�^
		double executionTime_[60] = {};

		// �t���[���J�E���^
		int timeCnt_ = 0;

		// ���O
		std::vector<std::string> vecLog_;

		// int�ϐ����
		std::unordered_map<const char*, int> mapTraceInt_;
		std::vector<const char*> intKeys_;

		// float�ϐ����
		std::unordered_map<const char*, float> mapTraceFloat_;
		std::vector<const char*> floatKeys_;

		// Vector2D�ϐ����
		std::unordered_map<const char*, Vector2D> mapTraceVector2D_;
		std::vector<const char*> vector2DKeys_;

		// Vector3D�ϐ����
		std::unordered_map<const char*, Vector3D> mapTraceVector3D_;
		std::vector<const char*> vector3DKeys_;

		// Vector4D�ϐ����
		std::unordered_map<const char*, Vector4D> mapTraceVector4D_;
		std::vector<const char*> vector4DKeys_;

		// ���\�[�X���
		std::vector<std::string> vecResource_;

		// �����̋֎~
		DebugManager();
		~DebugManager(){}

		// �R�s�[�R���X�g���N�^�̋֎~
		DebugManager(const DebugManager& src){}
		DebugManager& operator=(const DebugManager& src){}
	};
};

#endif	// #ifndef INCLUDE_IDEA_DEBUGMANAGER_H