/*==============================================================================
	[NetworkManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_NETWORKMANAGER_H
#define INCLUDE_IDEA_NETWORKMANAGER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

namespace idea{
	//------------------------------------------------------------------------------
	// �\���̒�`
	//------------------------------------------------------------------------------
#pragma pack(push,1)
	struct InputData{	// ���͏��
		short keyData;	// �L�[�̓���
		short lAxisX;	// ���A�i���O�X�e�B�b�N��X��
		short lAxisY;	// ���A�i���O�X�e�B�b�N��Y��
		short rAxisX;	// �E�A�i���O�X�e�B�b�N��X��
		short rAxisY;	// �E�A�i���O�X�e�B�b�N��Y��
		short lTrigger;	// ���g���K�[
		short rTrigger;	// �E�g���K�[
	};
#pragma pack(pop)

#pragma pack(push,1)
	struct Packet{
		union{
			char cw[128];
			short sw[64];	// short�^
			__int32 iw[32];	// int�^
			float fw[32];	// float�^
		};
	};
#pragma pack(pop)


#pragma pack(push,1)
	struct PacketData{	// �p�P�b�g���
		char PlayerNum;	// �v���C���[�ԍ�
		unsigned long long Cnt;
		unsigned long long Sequence;
		unsigned long long Serial;
		unsigned long  Seed;
		unsigned int Frequency;
		union{
			char cw[8];		// char�^
			short sw[4];	// short�^
			__int32 iw[2];	// int�^
			float fw[2];	// float�^
			double dw;		// double�^
		}Value[100];		// �l�̃f�[�^
		InputData id[30];	// �ߋ�30�t���[�����̓��̓f�[�^
	};
#pragma pack(pop)

	//------------------------------------------------------------------------------
	// �N���X���@�FNetworkManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@UDP�ʐM�̊Ǘ����s���N���X
	//------------------------------------------------------------------------------
	class NetworkManager{
	public:
		static NetworkManager& Instance()	// �B��̃C���X�^���X��Ԃ�
		{
			static NetworkManager s_Instance;
			return s_Instance;
		}

		bool Init();					// ������

		bool SetUpServer(u_short portNum);	// �T�[�o�[�̗����グ

		bool SetUpClient(u_short IPAddrA, u_short IPAddrB, u_short IPAddrC, u_short IPAddrD, u_short portNum);	// �N���C�A���g�̗����グ

		void Close();	// �ʐM�ؒf

		void UnInit();	// �I������

		bool IsSetUp()		// �ʐM�̗����グ���������Ă��邩�ǂ���
		{
			return bSetUp_;
		}

		bool IsServer()	// �T�[�o�[���ǂ���
		{
			return bServer_;
		}

		bool IsConnect()	// �N���C�A���g���ǂ���
		{
			return bConnect_;
		}

		bool IsCommunication()	// �ʐM�����ǂ���
		{
			return bCommunication_;
		}

		void Flash();	// ����M�f�[�^�̏���

		struct PacketData* GetSendData()		// ����f�[�^�̃|�C���^�̎擾
		{
			return &sendBuffer_;
		}

		struct PacketData* GetReceiveData()	// �󂯎��f�[�^�̃|�C���^�̎擾
		{
			return &receiveBuffer_;
		}

		unsigned long GetSeed();

		UINT GetFrequency()	// �ʐM�p�x�̎擾
		{
			return frequency_ / 3;
		}

		void WritingBegin()	// �������݃t���O�𗧂Ă�
		{
			bWrighting_ = true;
		}
		void WritingEnd()	// �������݃t���O������
		{
			bWrighting_ = false;
		}

		bool GetSkipFlag()
		{
			return bSkip_;
		}

		void SetSkipFlag(bool bSkip){
			bSkip_ = bSkip;
		}

	private:
		bool bNetwork_ = false;					// �ڑ�����
		SOCKET srcSocket_ = NULL;				// �����̃\�P�b�g
		SOCKET dstSocket_ = NULL;				// ����̃\�P�b�g
		struct sockaddr_in srcAddr_ = {};		// �����̃A�h���X
		struct sockaddr_in dstAddr_ = {};		// ����̃A�h���X
		int dstAddrSize_ = 0;					// ����̃A�h���X�̃T�C�Y
		fd_set readFds_ = {};					// �t�@�C���f�B�X�N���v�^�̃��X�g
		struct timeval tvConnect_ = {};			// �ڑ��ҋ@����
		struct timeval tv_ = {};				// �ʐM�ҋ@����
		bool bSetUp_ = false;					// �����グ�����t���O
		bool bServer_ = false;					// �T�[�o�[�t���O
		bool bConnect_ = false;					// �ڑ��t���O
		bool bCommunication_ = false;			// �ʐM���t���O
		bool bWrighting_ = false;				// �������݃t���O
		HANDLE hServerSendThread_ = NULL;		// �T�[�o�X���b�h�n���h��
		HANDLE hServerReceiveThread_ = NULL;	// �T�[�o�X���b�h�n���h��
		HANDLE hClientSendThread_ = NULL;		// �N���C�A���g���M�X���b�h�n���h��
		HANDLE hClientReceiveThread_ = NULL;	// �N���C�A���g��M�X���b�h�n���h��
		CRITICAL_SECTION cs_ = {};				// �N���e�B�J���Z�N�V����
		CRITICAL_SECTION cs2_ = {};				///
		struct PacketData sendBuffer_ = {};		// ���M�f�[�^
		struct PacketData tempReceiveBuffer_ = {};	// �ꎞ��M�f�[�^
		struct PacketData receiveBuffer_ = {};	// ��M�f�[�^
		unsigned long seed_ = 0U;
		int frequency_ = 0;	// �ʐM�p�x
		bool bSkip_ = false;
		unsigned long wait_ = 0U;

		friend unsigned __stdcall ServerSendThread(void* vp);		// �T�[�o�[���M�X���b�h
		friend unsigned __stdcall ServerReceiveThread(void* vp);	// �T�[�o�[��M�X���b�h
		friend unsigned __stdcall ClientSendThread(void* vp);		// �N���C�A���g���M�X���b�h
		friend unsigned __stdcall ClientReceiveThread(void* vp);	// �N���C�A���g��M�X���b�h

		void ServerSend();			// �T�[�o�[���M
		void ServerReceive();			// �T�[�o�[��M
		void ClientSend();		// �N���C�A���g���M
		void ClientReceive();	// �N���C�A���g��M

		NetworkManager();					// �R���X�g���N�^
		~NetworkManager(){ UnInit(); }		// �f�X�g���N�^
		// �R�s�[�R���X�g���N�^�̋֎~
		NetworkManager(const NetworkManager& src){}
		NetworkManager& operator=(const NetworkManager& src){}
	};
}

#endif