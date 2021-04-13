/*==============================================================================
	[Sound.cpp]
														Author	:	Keigo Hara
==============================================================================*/

#include "../../h/Sound/Sound.h"
#include "../../h/Sound/SoundManager.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Utility/DebugManager.h"
#include "../../h/Utility//ideaMath.h"
#include "../../h/Utility//ideaUtility.h"

#include <XAudio2.h>

using namespace idea;

HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition);	// �`�����N�̓ǂݍ���
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset);	// �`�����N�f�[�^�̓ǂݍ���

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
Sound::Sound() :
	pSourceVoice_(nullptr),
	pDataAudio_(nullptr),
	audioSize_(0U),
	bPlay_(false),
	bLoop_(false),
	bStorage_(false),
	bRegistered_(false),
	pPrev_(nullptr),
	pNext_(nullptr)
{
	ZeroMemory(fileName_, ArraySize(fileName_) * sizeof(char));
	SoundManager::Instance().Register(this);
}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
Sound::~Sound()
{
	UnLoad();	// �I������

	SoundManager::Instance().UnRegister(this);
}

//------------------------------------------------------------------------------
// wave�t�@�C������T�E���h��ǂݍ���
//------------------------------------------------------------------------------
bool Sound::LoadWaveFromFile(const char* pFileName)
{
	if(pSourceVoice_){ return false; }	//���ɓǂݍ��ݍς݂Ȃ�I��

	HANDLE hFile;				// �t�@�C���n���h��
	DWORD dwChunkSize = 0;		// �`�����N�T�C�Y
	DWORD dwChunkPosition = 0;	// �`�����N�̈ʒu
	DWORD dwFileType;			// �t�@�C���̎��
	WAVEFORMATEXTENSIBLE wfx = {0};	// �I�[�f�B�I �f�[�^�̃t�H�[�}�b�g
	XAUDIO2_BUFFER buffer = {0};		// �o�b�t�@

	// �T�E���h�f�[�^�t�@�C���̐���
	hFile = CreateFile(pFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER){	// �t�@�C���|�C���^��擪�Ɉړ�
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// WAVE�t�@�C���̃`�F�b�N
	if(FAILED(CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(FAILED(ReadChunkData(hFile, &dwFileType, sizeof(DWORD), dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(dwFileType != 'EVAW'){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �t�H�[�}�b�g�`�F�b�N
	if(FAILED(CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(FAILED(ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �I�[�f�B�I�f�[�^�ǂݍ���
	if(FAILED(CheckChunk(hFile, 'atad', &audioSize_, &dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	pDataAudio_ = (BYTE*)malloc(audioSize_);
	if(FAILED(ReadChunkData(hFile, pDataAudio_, audioSize_, dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �\�[�X�{�C�X�̐���
	if(!SoundManager::Instance().CreateSourceVoice(&pSourceVoice_, &(wfx.Format))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �o�b�t�@�̒l�ݒ�
	buffer.AudioBytes	= audioSize_;
	buffer.pAudioData	= pDataAudio_;
	buffer.Flags		= XAUDIO2_END_OF_STREAM;

	// �I�[�f�B�I�o�b�t�@�̓o�^
	pSourceVoice_->SubmitSourceBuffer(&buffer);

	// �I�[�f�B�I�o�b�t�@�̍폜
	pSourceVoice_->FlushSourceBuffers();

	CloseHandle(hFile);

	strcpy_s(fileName_, 255, pFileName);
	fileName_[255] = '\0';
	DebugManager::Instance().AddResource(fileName_);

	return true;
}

//------------------------------------------------------------------------------
// �A�[�J�C�u�t�@�C���̒���wave�t�@�C������T�E���h��ǂݍ���
//------------------------------------------------------------------------------
bool Sound::LoadWaveFromArchiveFile(const char * pArchiveFileName, const char * pFileName)
{
	if(pSourceVoice_){ return false; }	//���ɓǂݍ��ݍς݂Ȃ�I��

	ArchiveLoader loader;

	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("WaveLoadError! [%s] �� [%s] ����ǂݍ��߂܂���ł���\n", pFileName, pArchiveFileName);
		return false;
	}

	char tmpP[] = "p";
	char tempFileData[] = "wav_";
	char tmpM[] = "m";
	char tempFileNum[] = "000";
	char tmpT[] = "t";
	char tempFileName[256] = "idea_";
	char tmpDot[] = ".";

	strcat_s(tempFileName, tempFileData);
	strcat_s(tempFileName, tempFileNum);
	strcat_s(tempFileName, tmpDot);
	strcat_s(tempFileName, tmpT);
	strcat_s(tempFileName, tmpM);
	strcat_s(tempFileName, tmpP);

	{
		FILE* fp;
		if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

		if(fp == NULL) {
			return false;
		} else {
			fwrite(&loader.GetData()[0], loader.GetSize(), sizeof(BYTE), fp);
			fclose(fp);
		}
	}

	HANDLE hFile;				// �t�@�C���n���h��
	DWORD dwChunkSize = 0;		// �`�����N�T�C�Y
	DWORD dwChunkPosition = 0;	// �`�����N�̈ʒu
	DWORD dwFileType;			// �t�@�C���̎��
	WAVEFORMATEXTENSIBLE wfx = { 0 };	// �I�[�f�B�I �f�[�^�̃t�H�[�}�b�g
	XAUDIO2_BUFFER buffer = { 0 };		// �o�b�t�@

	// �T�E���h�f�[�^�t�@�C���̐���
	hFile = CreateFile(tempFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == INVALID_HANDLE_VALUE){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER){	// �t�@�C���|�C���^��擪�Ɉړ�
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// WAVE�t�@�C���̃`�F�b�N
	if(FAILED(CheckChunk(hFile, 'FFIR', &dwChunkSize, &dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(FAILED(ReadChunkData(hFile, &dwFileType, sizeof(DWORD), dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(dwFileType != 'EVAW'){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �t�H�[�}�b�g�`�F�b�N
	if(FAILED(CheckChunk(hFile, ' tmf', &dwChunkSize, &dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	if(FAILED(ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �I�[�f�B�I�f�[�^�ǂݍ���
	if(FAILED(CheckChunk(hFile, 'atad', &audioSize_, &dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}
	pDataAudio_ = (BYTE*)malloc(audioSize_);
	if(FAILED(ReadChunkData(hFile, pDataAudio_, audioSize_, dwChunkPosition))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �\�[�X�{�C�X�̐���
	if(!SoundManager::Instance().CreateSourceVoice(&pSourceVoice_, &(wfx.Format))){
		SetDebugMessage("WaveLoadError! [%s] ���t�@�C������ǂݍ��߂܂���ł���\n", pFileName);
		return false;
	}

	// �o�b�t�@�̒l�ݒ�
	buffer.AudioBytes = audioSize_;
	buffer.pAudioData = pDataAudio_;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	// �I�[�f�B�I�o�b�t�@�̓o�^
	pSourceVoice_->SubmitSourceBuffer(&buffer);

	// �I�[�f�B�I�o�b�t�@�̍폜
	pSourceVoice_->FlushSourceBuffers();

	CloseHandle(hFile);

	strcpy_s(fileName_, 255, pFileName);
	fileName_[255] = '\0';
	DebugManager::Instance().AddResource(fileName_);

	{
		FILE* fp;
		if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

		if(fp == NULL) {
			return false;
		} else {
			fwrite("", 1, sizeof(BYTE), fp);
		}

		fclose(fp);
	}

	DeleteFileA(tempFileName);

	return true;
}

//------------------------------------------------------------------------------
// �X�g���[�W����T�E���h��ǂݍ���
//------------------------------------------------------------------------------
bool Sound::LoadWaveFromStorage(const char* pFileName)
{
	if(pSourceVoice_){ return false; }	// ���ɓǂݍ��ݍς݂Ȃ�I��
	
	// �X�g���[�W�������ǂݍ���
	Sound& Snd = StorageManager::Instance().GetSound(pFileName);

	pSourceVoice_ = Snd.pSourceVoice_;
	pDataAudio_ = Snd.pDataAudio_;
	audioSize_ = Snd.audioSize_;

	if(pSourceVoice_ || pDataAudio_){ bStorage_ = true; }	// �X�g���[�W�g�p�t���O���I����
	else{
		pSourceVoice_ = nullptr;
		pDataAudio_ = nullptr;
		audioSize_ = 0;
		SetDebugMessage("WaveLoadError! [%s] ���X�g���[�W����ǂݍ��߂܂���ł���\n", pFileName);
	}

	strcpy_s(fileName_, 255, pFileName);
	fileName_[255] = '\0';

	return true;
}

//------------------------------------------------------------------------------
// �\�[�X�{�C�X�̔j��
//------------------------------------------------------------------------------
void Sound::UnLoad()
{
	if(!pSourceVoice_){ return; }

	Stop();	// �Đ����~�߂�

	if(!bStorage_){	// �X�g���[�W����ǂݍ��񂾂��̂̔j���̓X�g���[�W�ɔC����
		pSourceVoice_->DestroyVoice();
		if(pDataAudio_){ free(pDataAudio_); }
	}

	DebugManager::Instance().RemoveResource(fileName_);

	ZeroMemory(fileName_, ArraySize(fileName_) * sizeof(char));

	pSourceVoice_ = nullptr;
	pDataAudio_ = nullptr;
	audioSize_ = 0;
	bPlay_ = false;
	bLoop_ = false;
	bStorage_ = false;
}

//------------------------------------------------------------------------------
// ���y�̍Đ�
//------------------------------------------------------------------------------
void Sound::Play(bool bLoop, bool bCueing, UINT32 begin)
{
	if(!pSourceVoice_){ return; }	// �\�[�X�{�C�X�����݂��Ȃ���ΏI��

	bLoop_ = bLoop;	// ���[�v�t���O��ݒ�

	if(bCueing){ Cueing(); }	// ���o��

	if(!IsPlaying()){
		XAUDIO2_BUFFER buffer = {};	// �o�b�t�@

		// �o�b�t�@�̒l�ݒ�
		buffer.AudioBytes	= audioSize_;
		buffer.pAudioData	= pDataAudio_;
		buffer.PlayBegin	= begin;
		buffer.Flags		= XAUDIO2_END_OF_STREAM;
		buffer.LoopCount	= bLoop ? XAUDIO2_LOOP_INFINITE : 0;

		// �I�[�f�B�I�o�b�t�@�̓o�^
		if(FAILED(pSourceVoice_->SubmitSourceBuffer(&buffer))){ return; }
	}

	// �Đ�
	pSourceVoice_->Start(0);
	bPlay_ = true;
}

//------------------------------------------------------------------------------
// ���y�̒�~
//------------------------------------------------------------------------------
void Sound::Stop()
{
	if(!pSourceVoice_){ return; }	// �\�[�X�{�C�X�����݂��Ȃ���ΏI��

	// �ꎞ��~
	if(IsPlaying()){
		pSourceVoice_->Stop(0);

		bPlay_ = false;
	}
}

//------------------------------------------------------------------------------
// ���y�̉��y�̓��o��
//------------------------------------------------------------------------------
void Sound::Cueing()
{
	if(!pSourceVoice_){ return; }	// �\�[�X�{�C�X�����݂��Ȃ���ΏI��

	// �ꎞ��~
	if(IsPlaying()){
		Stop();
	}

	// �I�[�f�B�I�o�b�t�@�̍폜
	pSourceVoice_->FlushSourceBuffers();
}

//------------------------------------------------------------------------------
// ���ʂ̐ݒ�
//------------------------------------------------------------------------------
void Sound::SetVolume(float volume)
{
	if(!pSourceVoice_){ return; }	// �\�[�X�{�C�X�����݂��Ȃ���ΏI��

	volume = Max(0.0f, volume);

	pSourceVoice_->SetVolume(volume);	// ���ʂ̐ݒ�
}

//------------------------------------------------------------------------------
// �Đ�����
//------------------------------------------------------------------------------
bool Sound::IsPlaying()
{
	if(!pSourceVoice_){ return false; }	// �\�[�X�{�C�X�����݂��Ȃ����false

	XAUDIO2_VOICE_STATE voiceState;	// �\�[�X�{�C�X�̏��

	pSourceVoice_->GetState(&voiceState);	// ��Ԃ̎擾

	if(!voiceState.BuffersQueued){ bPlay_ = false; }	// ���ݏ������̃o�b�t�@�[���Ȃ���΍Đ����Ă��Ȃ�

	return bPlay_;	// ���݂̏���Ԃ�
}

//------------------------------------------------------------------------------
// ���[�v���Ă��邩
//------------------------------------------------------------------------------
bool Sound::IsLooping()
{
	if(!pSourceVoice_){ return false; }	// �\�[�X�{�C�X�����݂��Ȃ����false

	return bLoop_;	// ���݂̏���Ԃ�
}

//------------------------------------------------------------------------------
// �`�����N�̓ǂݍ���
//------------------------------------------------------------------------------
HRESULT CheckChunk(HANDLE hFile, DWORD format, DWORD* pChunkSize, DWORD* pChunkDataPosition)
{
	HRESULT hr = S_OK;			// ����
	DWORD dwRead;				// �ǂݍ��񂾈ʒu
	DWORD dwChunkType;			// �`�����N�̎��
	DWORD dwChunkDataSize;		// �`�����N�f�[�^�̃T�C�Y
	DWORD dwRIFFDataSize = 0;	// RIFF�f�[�^�̃T�C�Y
	DWORD dwFileType;			// �t�@�C���̎��
	DWORD dwBytesRead = 0;		// �o�C�g�T�C�Y
	DWORD dwOffset = 0;			// �I�t�Z�b�g
	
	if(SetFilePointer(hFile, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ return HRESULT_FROM_WIN32(GetLastError()); }	// �t�@�C���|�C���^��擪�Ɉړ�
	
	while(hr == S_OK){
		if(ReadFile(hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL) == 0)
		{ hr = HRESULT_FROM_WIN32(GetLastError()); }	// �`�����N�̓ǂݍ���

		if(ReadFile(hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL) == 0)
		{ hr = HRESULT_FROM_WIN32(GetLastError()); }	// �`�����N�f�[�^�̓ǂݍ���

		switch(dwChunkType){
		case 'FFIR':
			dwRIFFDataSize  = dwChunkDataSize;
			dwChunkDataSize = 4;
			if(ReadFile(hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL) == 0)
			{ hr = HRESULT_FROM_WIN32(GetLastError()); }	// �t�@�C���^�C�v�̓ǂݍ���
			break;

		default:
			if(SetFilePointer(hFile, dwChunkDataSize, NULL, FILE_CURRENT) == INVALID_SET_FILE_POINTER)
			{ return HRESULT_FROM_WIN32(GetLastError()); }	// �t�@�C���|�C���^���`�����N�f�[�^���ړ�
		}

		dwOffset += sizeof(DWORD) * 2;

		if(dwChunkType == format){
			*pChunkSize			= dwChunkDataSize;
			*pChunkDataPosition = dwOffset;

			return S_OK;
		}

		dwOffset += dwChunkDataSize;
		if(dwBytesRead >= dwRIFFDataSize){ return S_FALSE; }
	}
	
	return S_OK;
}

//------------------------------------------------------------------------------
// �`�����N�f�[�^�̓ǂݍ���
//------------------------------------------------------------------------------
HRESULT ReadChunkData(HANDLE hFile, void* pBuffer, DWORD dwBuffersize, DWORD dwBufferoffset)
{
	DWORD dwRead;	// �ǂݍ��񂾈ʒu
	
	if(SetFilePointer(hFile, dwBufferoffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{ return HRESULT_FROM_WIN32(GetLastError()); }	// �t�@�C���|�C���^���w��ʒu�܂ňړ�

	if(ReadFile(hFile, pBuffer, dwBuffersize, &dwRead, NULL) == 0)
	{ return HRESULT_FROM_WIN32(GetLastError()); }	// �f�[�^�̓ǂݍ���
	
	return S_OK;
}