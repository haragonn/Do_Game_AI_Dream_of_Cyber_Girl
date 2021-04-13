/*==============================================================================
	[Memory.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Storage/Memory.h"
#include "../../h/Storage/MemoryManager.h"

using namespace idea;


//------------------------------------------------------------------------------
// �L�[�ɐ�����ݒ�
//------------------------------------------------------------------------------
void Memory::SetInt(const char* pKey, int value)
{
	MemoryManager::Instance().SetInt(pKey, value);
}

//------------------------------------------------------------------------------
// �����̎擾
//------------------------------------------------------------------------------
int Memory::GetInt(const char* pKey, int def)
{
	if(!HasKeyInt(pKey)){ return def; }
	return MemoryManager::Instance().GetInt(pKey);
}

//------------------------------------------------------------------------------
// �w�肵���L�[�Ɗ֘A�t����ꂽ���������݂��邩
//------------------------------------------------------------------------------
bool Memory::HasKeyInt(const char* pKey)
{
	return MemoryManager::Instance().HasKeyInt(pKey);
}

//------------------------------------------------------------------------------
// �L�[�ɕ��������_����ݒ�
//------------------------------------------------------------------------------
void Memory::SetFloat(const char* pKey, float value)
{
	MemoryManager::Instance().SetFloat(pKey, value);
}

//------------------------------------------------------------------------------
// ���������_���̎擾
//------------------------------------------------------------------------------
float Memory::GetFloat(const char* pKey, float def)
{
	if(!HasKeyFloat(pKey)){ return def; }
	return MemoryManager::Instance().GetFloat(pKey);
}

//------------------------------------------------------------------------------
// �w�肵���L�[�Ɗ֘A�t����ꂽ���������_�������݂��邩
//------------------------------------------------------------------------------
bool Memory::HasKeyFloat(const char* pKey)
{
	return MemoryManager::Instance().HasKeyFloat(pKey);
}

//------------------------------------------------------------------------------
// �L�[�ɔ{���x���������_����ݒ�
//------------------------------------------------------------------------------
void Memory::SetDouble(const char* pKey, double value)
{
	MemoryManager::Instance().SetDouble(pKey, value);
}

//------------------------------------------------------------------------------
// �{���x���������_���̎擾
//------------------------------------------------------------------------------
double Memory::GetDouble(const char* pKey, double def)
{
	if(!HasKeyFloat(pKey)){ return def; }
	return MemoryManager::Instance().GetDouble(pKey);
}

//------------------------------------------------------------------------------
// �w�肵���L�[�Ɗ֘A�t����ꂽ�{���x���������_�������݂��邩
//------------------------------------------------------------------------------
bool Memory::HasKeyDouble(const char* pKey)
{
	return MemoryManager::Instance().HasKeyDouble(pKey);
}
