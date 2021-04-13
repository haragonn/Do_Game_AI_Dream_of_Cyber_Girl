/*==============================================================================
	[ideaUtility.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_IDEAUTILITY_H
#define INCLUDE_IDEA_IDEAUTILITY_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <stdlib.h>
#include <crtdbg.h>
#include <string>
#include <vector>
#include <codecvt>
#include <locale>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#define new new(_NORMAL_BLOCK,__FILE__,__LINE__)
#define SetDebugMessage(str,...){ TCHAR c[1024];_stprintf_s(c,(sizeof(c)/sizeof(c[0])),_T(str),__VA_ARGS__);OutputDebugString(c); }
#define SetLeakCheckFlag(){ _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF); }
#define GetHeapSize(){ SetDebugMessage("/*==============================================================================\n\t\t%s(%d)\n================================================================================\n",__FILE__,__LINE__);_CrtDumpMemoryLeaks();SetDebugMessage("=================================================================================\n\t\t%s(%d)\n===============================================================================*/\n",__FILE__,__LINE__); }
#define Assert(exp){ _ASSERT(exp); }
#else
#define SetDebugMessage(str,...)
#define SetLeakCheckFlag()
#define GetHeapSize()
#define Assert(exp)
#endif

namespace idea{
	// �Z�[�t�f���[�g
	template <typename T>
	inline void SafeDelete(T*& p)
	{
		if(p){
			delete p;
			p = nullptr;
		}
	}

	// �z��̃Z�[�t�f���[�g
	template <typename T>
	inline void SafeDeleteArray(T*& p)
	{
		if(p){
			delete[] p;
			p = nullptr;
		}
	}

	// �Z�[�t�����[�X
	template <typename T>
	inline void SafeRelease(T*& p)
	{
		if(p){
			p->Release();
			p = nullptr;
		}
	}

	// �Z�[�t�f�X�g���C
	template <typename T>
	inline void SafeDestroy(T*& p)
	{
		if(p){
			p->Destroy();
			p = nullptr;
		}
	}

	// �z��̃T�C�Y�擾
	template <typename T, size_t SIZE>
	inline size_t ArraySize(const T(&)[SIZE])
	{
		return SIZE;
	}

	// �t�@�C���l�[���Ƀp�X��ǉ�
	inline void AddDirectoryPath(std::string& fileName, const std::string& path)
	{
		int n = path.rfind("/");

		if(n < 0){
			const char* t = path.c_str();

			for(int i = path.size() - 1; i >= 1; --i){
				if(t[i] == '\\'){
					fileName = path.substr(0, i - 1) + fileName;

					break;
				}
			}
		} else{
			fileName = path.substr(0, n + 1) + fileName;
		}
	}

	// wstring��string�ɕϊ�
	inline std::string WStringToString(const std::wstring& wstring)
	{
		setlocale(LC_ALL, "Japanese");

		int bufferSize = WideCharToMultiByte(CP_OEMCP, 0, wstring.c_str(), -1, (char*)NULL, 0, NULL, NULL);

		CHAR* pMultiByte = new CHAR[bufferSize];

		WideCharToMultiByte(CP_OEMCP, 0, wstring.c_str(), -1, pMultiByte, bufferSize, NULL, NULL);

		std::string res(pMultiByte, pMultiByte + bufferSize - 1);

		SafeDeleteArray(pMultiByte);

		return res;
	}

	// string��wstring�ɕϊ�
	inline std::wstring StringToWString(const std::string& string)
	{
		setlocale(LC_ALL, "Japanese");

		int bufferSize = MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, (wchar_t*)NULL, 0);

		wchar_t* pUCS2 = new wchar_t[bufferSize];

		MultiByteToWideChar(CP_ACP, 0, string.c_str(), -1, pUCS2, bufferSize);

		std::wstring res(pUCS2, pUCS2 + bufferSize - 1);

		SafeDeleteArray(pUCS2);

		return res;
	}

	// ���C�h������UFT8�ɕϊ�
	inline std::string WideToUFT8(const std::wstring& wide)
	{
		setlocale(LC_ALL, "Japanese");

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		return converter.to_bytes(wide);
	}

	// �}���`�o�C�g������UFT8�ɕϊ�
	inline std::string MultiToUTF8(const std::string& multi)
	{
		setlocale(LC_ALL, "Japanese");

		auto const wide = StringToWString(multi);
		return WideToUFT8(wide);
	}

	// �t�H�[�}�b�g�`���̕�����̍쐬
	template <typename ... Args>
	inline std::string FormatString(const std::string& fmt, Args ... args)
	{
		setlocale(LC_ALL, "Japanese");

		size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
		std::vector<char> buf(len + 1);
		std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
		return std::string(&buf[0], &buf[0] + len);
	}

	// ��ԑJ�ڃN���X
	class Phase{
	public:
		// �t�F�C�Y�̍ő�l
		static const int PHASE_MAX = 64;

		// �t�F�C�Y�I���̒l
		static const int END_OF_PHASE = -1;

		// ������
		void Init(int phaseMax, bool bLoop = false)
		{
			phaseMax_ = (PHASE_MAX < phaseMax) ? PHASE_MAX : phaseMax;

			phase_ = 1;

			for(int i = phaseMax_ - 1; i >= 0; --i){
				countMax_[i] = 0;
			}

			bLoop_ = bLoop;
		}

		// �t�F�C�Y���Ƃ̃J�E���g����̐ݒ�
		void SetCountMax(int phase, int cntMax)
		{
			if(phase <= 0 || phase > phaseMax_){
				return;
			}

			countMax_[phase - 1] = cntMax;
		}

		// �t�F�C�Y�̐؂�ւ�
		void SetPhase(int phase)
		{
			if(phase <= 0 || phase > phaseMax_){
				return;
			}

			phase_ = phase;
			count_ = 0;
		}

		// �t�F�C�Y�̎擾
		int GetPhase()const{ return phase_; }

		// �J�E���g��i�߂�
		int AddCount()
		{
			if(phase_ < 0){
				return phase_;
			}

			++count_;

			if(count_ >= countMax_[phase_ - 1]){
				if(phase_ < phaseMax_){
					++phase_;
					count_ = 0;
				} else{
					if(bLoop_){
						phase_ = 1;
					} else{
						phase_ = -1;
					}
				}
			}

			return phase_;
		}

		// �J�E���g��i�߂�(�O�u�C���N�������g)
		int operator ++(){
			return AddCount();
		}

		// �J�E���g��i�߂�(��u�C���N�������g)
		int operator ++(int){
			if(phase_ < 0){
				return phase_;
			}

			int tmp = phase_;

			AddCount();

			return tmp;
		}

	private:
		// �t�F�C�Y�̏��
		int phaseMax_ = 0;

		// �t�F�C�Y
		int phase_ = END_OF_PHASE;

		// �e�t�F�C�Y�̃J�E���g���
		int countMax_[PHASE_MAX] = {};

		// �J�E���g
		int count_ = 0;

		// ���[�v�t���O
		bool bLoop_ = false;
	};

}

#endif	// #ifndef INCLUDE_IDEA_IDEAUTILITY_H