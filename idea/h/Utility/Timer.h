/*==============================================================================
	[Timer.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_TIMER_H
#define INCLUDE_IDEA_TIMER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <mmsystem.h>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FTimer
	// �N���X�T�v�F���Ԍv�����s���N���X
	//------------------------------------------------------------------------------
	class Timer{
	public:
		Timer();
		~Timer();

		// �v���J�n�����̐ݒ�
		void SetStartTime()
		{
			dwStart_ = timeGetTime();
			if(bPerformanceCounter_){ QueryPerformanceCounter(&liStart_); }
		}

		// �o�ߎ��Ԃ̎擾
		unsigned long GetElapsed()
		{
			return static_cast<unsigned long>(timeGetTime() - dwStart_);
		}

		// �����x�Ȍo�ߎ��Ԃ̎擾
		double GetHighPrecisionElapsed()
		{
			if(bPerformanceCounter_){
				QueryPerformanceCounter(&liNow_);
				return (liNow_.QuadPart - liStart_.QuadPart) * clock_;
			}
			return static_cast<double>(timeGetTime() - dwStart_);
		}

	private:
		// timeBeginPeriod()�����t���O
		static bool s_bSetPeriod_;

		// �J�n����
		DWORD dwStart_ = 0U;

		// ����
		LARGE_INTEGER liStart_ = {};
		LARGE_INTEGER liNow_ = {};
		LARGE_INTEGER liFreq_ = {};

		// 1�N���b�N�P��
		double clock_ = 0.0;

		// �����x�^�C�}�[�����݂��邩
		bool bPerformanceCounter_ = false;
	};
}

#endif // #ifndef INCLUDE_IDEA_TIMER_H