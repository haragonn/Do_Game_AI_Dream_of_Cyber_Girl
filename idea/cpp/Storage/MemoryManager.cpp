/*==============================================================================
	[MemoryManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/MemoryManager.h"

using namespace idea;

//------------------------------------------------------------------------------
// �������L�[�Ɋ֘A�t��
//------------------------------------------------------------------------------
void MemoryManager::SetInt(const char* pKey, int value)
{
	if(HasKeyInt(pKey)){	// ���ɃL�[���o�^����Ă����
		mapInt_[pKey] = value;	// �l���X�V
		return;
	}
	mapInt_.insert(std::unordered_map<const char*, int>::value_type(pKey, value));	// �V�K�o�^
}

//------------------------------------------------------------------------------
// �����̎擾
//------------------------------------------------------------------------------
int MemoryManager::GetInt(const char* pKey)
{
	if(!HasKeyInt(pKey)){ return 0; }	// �L�[�����݂��Ȃ����0��Ԃ�
	return mapInt_[pKey];	// �l��Ԃ�
}

//------------------------------------------------------------------------------
// �w�肵���L�[�Ɗ֘A�t����ꂽ���������݂��邩
//------------------------------------------------------------------------------
bool MemoryManager::HasKeyInt(const char* pKey)
{
	return (mapInt_.find(pKey) != mapInt_.end());	// ����
}

//------------------------------------------------------------------------------
// ���������_�����L�[�Ɋ֘A�t��
//------------------------------------------------------------------------------
void MemoryManager::SetFloat(const char* pKey, float value)
{
	if(HasKeyFloat(pKey)){	// ���ɃL�[���o�^����Ă����
		mapFloat_[pKey] = value;	// �l���X�V
		return;
	}
	mapFloat_.insert(std::unordered_map<const char*, float>::value_type(pKey, value));	// �V�K�o�^
}

//------------------------------------------------------------------------------
// ���������_���̎擾
//------------------------------------------------------------------------------
float MemoryManager::GetFloat(const char* pKey)
{
	if(!HasKeyFloat(pKey)){ return 0.0f; }	// �L�[�����݂��Ȃ����0.0f��Ԃ�
	return mapFloat_[pKey];	// �l��Ԃ�
}

//------------------------------------------------------------------------------
// �w�肵���L�[�Ɗ֘A�t����ꂽ���������_�������݂��邩
//------------------------------------------------------------------------------
bool MemoryManager::HasKeyFloat(const char* pKey)
{
	return (mapFloat_.find(pKey) != mapFloat_.end());	// ����
}

//------------------------------------------------------------------------------
// �{���x���������_�����L�[�Ɋ֘A�t��
//------------------------------------------------------------------------------
void MemoryManager::SetDouble(const char* pKey, double value)
{
	if(HasKeyDouble(pKey)){	// ���ɃL�[���o�^����Ă����
		mapDouble_[pKey] = value;	// �l���X�V
		return;
	}
	mapDouble_.insert(std::unordered_map<const char*, double>::value_type(pKey, value));	// �V�K�o�^
}

//------------------------------------------------------------------------------
// �{���x���������_���̎擾
//------------------------------------------------------------------------------
double MemoryManager::GetDouble(const char* pKey)
{
	if(!HasKeyDouble(pKey)){ return 0.0; }	// �L�[�����݂��Ȃ����0.0��Ԃ�
	return mapDouble_[pKey];	// �l��Ԃ�
}

//------------------------------------------------------------------------------
// �w�肵���L�[�Ɗ֘A�t����ꂽ�{���x���������_�������݂��邩
//------------------------------------------------------------------------------
bool MemoryManager::HasKeyDouble(const char* pKey)
{
	return (mapDouble_.find(pKey) != mapDouble_.end());	// ����
}