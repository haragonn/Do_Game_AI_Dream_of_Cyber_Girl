#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "../../h/Utility/ImGuiManager.h"
#include "../../h/Framework/GraphicManager.h"

using namespace idea;
using namespace ImGui;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool ImGuiManager::Init()
{
	GraphicManager& gm = GraphicManager::Instance();

	if(!gm.GetDevicePtr()
	|| !gm.GetContextPtr()){
		return false;
	}

	bool hr = false;

	hr = CreateContext();
	if(!hr){ return false; }

	hr = ImGui_ImplWin32_Init(gm.GetHWND());
	if(!hr){ return false; }

	hr = ImGui_ImplDX11_Init(gm.GetDevicePtr(), gm.GetContextPtr());
	if(!hr){ return false; }

	ImGui::StyleColorsDark();
	ImGuiIO& io = ImGui::GetIO();

	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // �L�[�{�[�h�ł�UI������\�ɂ���

	const char* filename = "font/mplus-1p-medium.ttf"; // ���{��̃t�H���g
	float size_pixels = 30.0f; // �t�H���g�T�C�Y

	// .ttf�̕����̒�����GetGlyphRangesJapanese�Ŏw�肳�ꂽ�����Ȃǂ̕�����imgui�̒��Ŏg����悤�ɂ��܂��B
	ImFont* font = io.Fonts->AddFontFromFileTTF(filename, size_pixels, NULL, io.Fonts->GetGlyphRangesJapanese());
	IM_ASSERT(font != NULL);
	io.FontGlobalScale = 0.6f; // �t�H���g�̑傫�����ꊇ�ŕύX�ł��܂��B
	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(0.6f); // UI�̑傫�����ꊇ�ŕύX�ł��܂��B

	return true;
}

void ImGuiManager::UnInit()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	DestroyContext();
}

void ImGuiManager::Update()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	NewFrame();
}

void ImGuiManager::Draw()
{
	Render();
	ImGui_ImplDX11_RenderDrawData(GetDrawData());
}

void ImGuiManager::SetWndProcHandler(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, msg, wp, lp);
}
