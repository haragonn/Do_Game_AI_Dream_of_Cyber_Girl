/*==============================================================================
	[DebugManager.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "../../h/Utility/DebugManager.h"
#include "../../h/Framework/FrameworkManager.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/Utility/IniReader.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;
using namespace idea;
using namespace ImGui;

DebugManager::DebugManager() :
	bDraw_(false),
	bShowLog_(false),
	bShowTrace_(false),
	bShowResource_(false),
	frame_(0U),
	timeCnt_(0)
{
	ZeroMemory(executionTime_, ArraySize(executionTime_));
}

bool DebugManager::Init()
{
	vector<string>().swap(vecResource_);

	IniReader ir("debug.ini");

	bDraw_ = !!(ir.ReadInt("IMGUI", "SHOW", 0));
	for(int i = 0; i < 3; ++i){
		bDebugSwitch_[i] = false;
	}
	bShowLog_ = !!(ir.ReadInt("IMGUI", "LOG", 0));
	bShowTrace_ = !!(ir.ReadInt("IMGUI", "TRACE", 0));
	bShowResource_ = !!(ir.ReadInt("IMGUI", "RESOURCE", 0));

	controller_.Init(0, true);
	controller_.SetConfig(PadButton::A, KeyCode::F1);
	controller_.SetConfig(PadButton::B, KeyCode::F2);
	controller_.SetConfig(PadButton::X, KeyCode::F3);
	controller_.SetConfig(PadButton::Y, KeyCode::F4);

	tmr_.SetStartTime();

	frame_ = 0U;

	return true;
}

void DebugManager::UnInit()
{
	IniReader ir("debug.ini");
	ir.WriteInt("IMGUI", "SHOW", bDraw_ ? 1 : 0);
	ir.WriteInt("IMGUI", "LOG", bShowLog_ ? 1 : 0);
	ir.WriteInt("IMGUI", "TRACE", bShowTrace_ ? 1 : 0);
	ir.WriteInt("IMGUI", "RESOURCE", bShowResource_ ? 1 : 0);
}

void DebugManager::Update(bool bResourceDraw)
{
	FrameworkManager& fm = FrameworkManager::Instance();
	GraphicManager& gm = GraphicManager::Instance();

	controller_.Update();

	//if(controller_.GetButton(PadButton::A) == 1){
	//	bDraw_ = !bDraw_;
	//}

	if(controller_.GetButton(PadButton::A) == 1){
		bDebugSwitch_[0] = !bDebugSwitch_[0];
	}

	if(controller_.GetButton(PadButton::B) == 1){
		bDebugSwitch_[1] = !bDebugSwitch_[1];
	}

	if(controller_.GetButton(PadButton::X) == 1){
		bDebugSwitch_[2] = !bDebugSwitch_[2];
	}

	if(bDraw_){
		PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.5f));
		PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 0.8f));
		
		SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_::ImGuiCond_Once);
		SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_::ImGuiCond_Once);

		ImGuiWindowFlags flag = 0;
		flag |= ImGuiWindowFlags_NoResize;
		flag |= ImGuiWindowFlags_NoMove;
		flag |= ImGuiWindowFlags_AlwaysAutoResize;

		Begin("System", nullptr, flag);

		unsigned long h = FrameToHour(frame_, fm.GetTargetFps());
		unsigned long m = FrameToMinute(frame_, fm.GetTargetFps()) % 60U;
		unsigned long s = FrameToSecond(frame_, fm.GetTargetFps()) % 60U;

		static int radio = 0;

		RadioButton(u8"フレーム", &radio, 0);
		SameLine();
		RadioButton(u8"実時間", &radio, 1);

		if(radio == 0){
			string time = u8"実行時間：" + FormatString("%09d", frame_);
			Text(time.c_str());
		} else if(radio == 1){
			string time = u8"実行時間：" + FormatString(u8"%02d：%02d：%02d", h, m, s);
			Text(time.c_str());
		}

		executionTime_[timeCnt_++ % 60] = fm.GetUpdateTime();

		static double ave = 0.0;

		static float fps = 0.0f;

		if(timeCnt_ % 60 == 0){
			fps = fm.GetFps();
			for(int i = 0; i < 60; ++i){
				ave += executionTime_[i];
			}

			ave /= 60.0;
		}

		string updateTime = u8"更新時間：" + FormatString("%.2lf", ave) + " [ms]";
		Text(updateTime.c_str());


		string fpsText = u8"fps：" + FormatString("%.4f", fps);
		Text(fpsText.c_str());

		Checkbox("Log", &bShowLog_);

		Checkbox("Trace", &bShowTrace_);

		Checkbox("Resource", &bShowResource_);

		End();

		int diff = 0;

		if(bResourceDraw && bShowLog_)
		{
			SetNextWindowSize(ImVec2(220.0f, (float)gm.GetHeight() / 2.0f), ImGuiCond_::ImGuiCond_Once);
			SetNextWindowPos(ImVec2((float)gm.GetWidth() - 220.0f - 220.0f * diff, 0.0f), ImGuiCond_::ImGuiCond_Always);

			ImGuiWindowFlags flag = 0;
			bool bOpen = true;

			Begin("Log", nullptr, flag);

			if(vecLog_.size()){
				reverse(begin(vecLog_), end(vecLog_));
				for(auto& rc : vecLog_){
					Text(rc.c_str());
				}
				reverse(begin(vecLog_), end(vecLog_));
			}

			End();

			++diff;
		}

		if(bResourceDraw && bShowTrace_)
		{
			SetNextWindowSize(ImVec2(220.0f, (float)gm.GetHeight() / 2.0f), ImGuiCond_::ImGuiCond_Once);
			SetNextWindowPos(ImVec2((float)gm.GetWidth() - 220.0f - 220.0f * diff, 0.0f), ImGuiCond_::ImGuiCond_Always);

			ImGuiWindowFlags flag = 0;
			bool bOpen = true;

			Begin("Trace", nullptr, flag);

			if(mapTraceInt_.size()){
				for(auto& key : intKeys_){
					string str = key;
					str = "(int) " + str + "\n\t\t" + to_string(mapTraceInt_[key]);
					Text(MultiToUTF8(str).c_str());
				}
			}

			if(mapTraceFloat_.size()){
				for(auto& key : floatKeys_){
					string str = key;
					str = "(float) " + str + "\n\t\t" + to_string(mapTraceFloat_[key]);
					Text(MultiToUTF8(str).c_str());
				}
			}

			if(mapTraceVector2D_.size()){
				for(auto& key : vector2DKeys_){
					string str = key;
					str = "(Vector2D) " + str + "\n\tx: " + to_string(mapTraceVector2D_[key].x) + "\n\ty: " + to_string(mapTraceVector2D_[key].y);
					Text(MultiToUTF8(str).c_str());
				}
			}

			if(mapTraceVector3D_.size()){
				for(auto& key : vector3DKeys_){
					string str = key;
					str = "(Vector3D) " + str + "\n\tx: " + to_string(mapTraceVector3D_[key].x) + "\n\ty: " + to_string(mapTraceVector3D_[key].y) + "\n\tz: " + to_string(mapTraceVector3D_[key].z);
					Text(MultiToUTF8(str).c_str());
				}
			}

			if(mapTraceVector4D_.size()){
				for(auto& key : vector4DKeys_){
					string str = key;
					str = "(Vector4D) " + str + "\n\tx: " + to_string(mapTraceVector4D_[key].x) + "\n\ty: " + to_string(mapTraceVector4D_[key].y) + "\n\tz: " + to_string(mapTraceVector4D_[key].z) + "\n\tw: " + to_string(mapTraceVector4D_[key].w);
					Text(MultiToUTF8(str).c_str());
				}
			}

			End();

			++diff;
		}

		if(bResourceDraw && bShowResource_)
		{
			SetNextWindowSize(ImVec2(220.0f, (float)gm.GetHeight() / 2.0f), ImGuiCond_::ImGuiCond_Once);
			SetNextWindowPos(ImVec2((float)gm.GetWidth() - 220.0f - 220.0f * diff, 0.0f), ImGuiCond_::ImGuiCond_Always);

			ImGuiWindowFlags flag = 0;
			bool bOpen = true;

			Begin("Resource", nullptr, flag);

			if(vecResource_.size()){
				reverse(begin(vecResource_), end(vecResource_));
				for(auto& rc : vecResource_){
					Text(rc.c_str());
				}
				reverse(begin(vecResource_), end(vecResource_));
			}

			End();

			++diff;
		}

		ImGui::PopStyleColor();
		ImGui::PopStyleColor();
	}

	++frame_;
}

bool  DebugManager::GetDebugSwitch(int index) const
{
	if(!bDraw_){
		return false;
	}

	if(index < 0 || index > 2){
		return false;
	}

	return bDebugSwitch_[index];
}

void DebugManager::SetDebugSwitch(int index, bool flag)
{
	if(index < 0 || index > 2){
		return;
	}

	bDebugSwitch_[index] = flag;
}

void  DebugManager::Log(std::string str)
{
	vecLog_.push_back(MultiToUTF8(str));
}

void  DebugManager::ClearLog()
{
	std::vector<std::string>().swap(vecLog_);
}

void  DebugManager::Trace(const char* pVariableName, int value)
{
	if(mapTraceInt_.find(pVariableName) == mapTraceInt_.end()){
		mapTraceInt_.insert(pair<const char*, int>{ pVariableName, 0 });
		intKeys_.push_back(pVariableName);
	}

	auto it = mapTraceInt_.find(pVariableName);
	if(it != mapTraceInt_.end()){
		it->second = value;
	}
}

void  DebugManager::Trace(const char* pVariableName, float value)
{
	if(mapTraceFloat_.find(pVariableName) == mapTraceFloat_.end()){
		mapTraceFloat_.insert(pair<const char*, float>{ pVariableName, 0.0f });
		floatKeys_.push_back(pVariableName);
	}

	auto it = mapTraceFloat_.find(pVariableName);
	if(it != mapTraceFloat_.end()){
		it->second = value;
	}
}

void  DebugManager::Trace(const char* pVariableName, const Vector2D& value)
{
	if(mapTraceVector2D_.find(pVariableName) == mapTraceVector2D_.end()){
		mapTraceVector2D_.insert(pair<const char*, Vector2D>{ pVariableName, Vector2D() });
		vector2DKeys_.push_back(pVariableName);
	}

	auto it = mapTraceVector2D_.find(pVariableName);
	if(it != mapTraceVector2D_.end()){
		it->second = value;
	}
}

void  DebugManager::Trace(const char* pVariableName, const Vector3D& value)
{
	if(mapTraceVector3D_.find(pVariableName) == mapTraceVector3D_.end()){
		mapTraceVector3D_.insert(pair<const char*, Vector3D>{ pVariableName, Vector3D() });
		vector3DKeys_.push_back(pVariableName);
	}

	auto it = mapTraceVector3D_.find(pVariableName);
	if(it != mapTraceVector3D_.end()){
		it->second = value;
	}
}

void  DebugManager::Trace(const char* pVariableName, const Vector4D& value)
{
	if(mapTraceVector4D_.find(pVariableName) == mapTraceVector4D_.end()){
		mapTraceVector4D_.insert(pair<const char*, Vector4D>{ pVariableName, Vector4D() });
		vector4DKeys_.push_back(pVariableName);
	}

	auto it = mapTraceVector4D_.find(pVariableName);
	if(it != mapTraceVector4D_.end()){
		it->second = value;
	}
}

void  DebugManager::ClearTrace()
{
	std::unordered_map<const char*, int>().swap(mapTraceInt_);
	std::vector<const char*>().swap(intKeys_);

	std::unordered_map<const char*, float>().swap(mapTraceFloat_);
	std::vector<const char*>().swap(floatKeys_);

	std::unordered_map<const char*, Vector2D>().swap(mapTraceVector2D_);
	std::vector<const char*>().swap(vector2DKeys_);

	std::unordered_map<const char*, Vector3D>().swap(mapTraceVector3D_);
	std::vector<const char*>().swap(vector3DKeys_);

	std::unordered_map<const char*, Vector4D>().swap(mapTraceVector4D_);
	std::vector<const char*>().swap(vector4DKeys_);

}

void  DebugManager::AddResource(const char* pFileName)
{
	if(find(vecResource_.begin(), vecResource_.end(), pFileName) == vecResource_.end()){
		string str = pFileName;
		vecResource_.push_back(MultiToUTF8(str));
	}
}

void  DebugManager::RemoveResource(const char* pFileName)
{
	auto it = find(vecResource_.begin(), vecResource_.end(), pFileName);
	if(it != vecResource_.end()){
		vecResource_.erase(it);
	}
}

void  DebugManager::RefreshTime()
{
	tmr_.SetStartTime();
	frame_ = 0U;
}