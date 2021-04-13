#ifndef INCLUDE_IDEA_IMGUIMANAGER_H
#define INCLUDE_IDEA_IMGUIMANAGER_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FImGuiManager
	// �N���X�T�v�F�V���O���g��
	// �@�@�@�@�@�@ImGui�̊Ǘ����s��
	//------------------------------------------------------------------------------
	class ImGuiManager{
	public:
		// �B��̃C���X�^���X
		static ImGuiManager& Instance()
		{
			static ImGuiManager s_Instance;
			return s_Instance;
		}

		// ������
		bool Init();

		// �I������
		void UnInit();

		// �X�V
		void Update();

		// �`��
		void Draw();

		// �v���V�[�W��
		void SetWndProcHandler(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	private:
		// �����̋֎~
		ImGuiManager(){}
		~ImGuiManager(){}

		// �R�s�[�R���X�g���N�^�̋֎~
		ImGuiManager(const ImGuiManager& src){}
		ImGuiManager& operator=(const ImGuiManager& src){}
	};
};

#endif	// #ifndef INCLUDE_IDEA_DEBUGMANAGER_H