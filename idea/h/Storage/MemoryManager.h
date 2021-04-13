/*==============================================================================
	[MemoryManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_MEMORYMANAGER_H
#define INCLUDE_IDEA_MEMORYMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <unordered_map>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FMemoryManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@�V�[�����܂����Œl��ێ�����N���X
	//------------------------------------------------------------------------------
	class MemoryManager{
	public:
		// �B��̃C���X�^���X��Ԃ�
		static MemoryManager& Instance(void)
		{
			static MemoryManager s_Instance;
			return s_Instance;
		}
		
		// �������L�[�Ɋ֘A�t��
		void SetInt(const char* pKey, int value);
		
		// �����̎擾
		int GetInt(const char* pKey);
		
		// �w�肵���L�[�Ɗ֘A�t����ꂽ���������݂��邩
		bool HasKeyInt(const char* pKey);

		// ���������_�����L�[�Ɋ֘A�t��
		void SetFloat(const char* pKey, float value);
		
		// ���������_���̎擾
		float GetFloat(const char* pKey);
		
		// �w�肵���L�[�Ɗ֘A�t����ꂽ���������_�������݂��邩
		bool HasKeyFloat(const char* pKey);

		// �{���x���������_�����L�[�Ɋ֘A�t��
		void SetDouble(const char* pKey, double value);
		
		// �{���x���������_���̎擾
		double GetDouble(const char* pKey);
		
		// �w�肵���L�[�Ɗ֘A�t����ꂽ�{���x���������_�������݂��邩
		bool HasKeyDouble(const char* pKey);

	private:
		// �����̃}�b�v
		std::unordered_map<const char*, int> mapInt_;
		
		// ���������_���̃}�b�v
		std::unordered_map<const char*, float> mapFloat_;
		
		// �{���x���������_���̃}�b�v
		std::unordered_map<const char*, double> mapDouble_;

		MemoryManager(){}
		~MemoryManager(){}

		// �R�s�[�R���X�g���N�^�̋֎~
		MemoryManager(const MemoryManager& src){}
		MemoryManager& operator=(const MemoryManager& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_MEMORYMANAGER_H