#ifndef INCLUDE_IDEA_IMGUIMANAGER_H
#define INCLUDE_IDEA_IMGUIMANAGER_H
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：ImGuiManager
	// クラス概要：シングルトン
	// 　　　　　　ImGuiの管理を行う
	//------------------------------------------------------------------------------
	class ImGuiManager{
	public:
		// 唯一のインスタンス
		static ImGuiManager& Instance()
		{
			static ImGuiManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init();

		// 終了処理
		void UnInit();

		// 更新
		void Update();

		// 描画
		void Draw();

		// プロシージャ
		void SetWndProcHandler(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp);

	private:
		// 生成の禁止
		ImGuiManager(){}
		~ImGuiManager(){}

		// コピーコンストラクタの禁止
		ImGuiManager(const ImGuiManager& src){}
		ImGuiManager& operator=(const ImGuiManager& src){}
	};
};

#endif	// #ifndef INCLUDE_IDEA_DEBUGMANAGER_H