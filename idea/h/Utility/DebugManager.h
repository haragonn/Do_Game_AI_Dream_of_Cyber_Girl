/*==============================================================================
	[DebugManager.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_DEBUGMANAGER_H
#define INCLUDE_IDEA_DEBUGMANAGER_H

#include "../Input/Controller.h"
#include "../Utility/Timer.h"
#include "../Utility/ideaMath.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：DebugManager
	// クラス概要：シングルトン
	// 　　　　　　デバッグ情報の管理を行う
	//------------------------------------------------------------------------------
	class DebugManager{
	public:
		// 唯一のインスタンス
		static DebugManager& Instance()
		{
			static DebugManager s_Instance;
			return s_Instance;
		}

		// 初期化
		bool Init();

		// 終了処理
		void UnInit();

		// 更新
		void Update(bool bResourceDraw);

		// デバッグ表示中か
		bool IsDebugMode()const
		{
			return bDraw_;
		}

		// デバッグスイッチがオンか
		bool GetDebugSwitch(int index)const;

		// デバッグスイッチの切り替え
		void SetDebugSwitch(int index, bool flag);

		// ログの書き込み
		void Log(std::string str);

		// ログのクリア
		void ClearLog();

		// 変数のトレース
		void Trace(const char* pVariableName, int value);

		// 変数のトレース
		void Trace(const char* pVariableName, float value);

		// 変数のトレース
		void Trace(const char* pVariableName, const Vector2D& value);

		// 変数のトレース
		void Trace(const char* pVariableName, const Vector3D& value);

		// 変数のトレース
		void Trace(const char* pVariableName, const Vector4D& value);

		// トレース情報のクリア
		void ClearTrace();

		// リソース情報の追加
		void AddResource(const char* pFileName);

		// リソース情報の破棄
		void RemoveResource(const char* pFileName);

		// タイマーの初期化
		void RefreshTime();

	private:
		// コントローラー
		Controller controller_;

		// タイマー
		Timer tmr_;

		// 描画フラグ
		bool bDraw_ = false;

		// デバッグスイッチ
		bool bDebugSwitch_[3] = {};

		// ログ描画フラグ
		bool bShowLog_ = false;

		// int変数描画フラグ
		bool bShowTrace_ = false;

		// リソース情報描画フラグ
		bool bShowResource_ = false;

		// 累計フレーム
		unsigned int frame_ = 0U;

		// 更新時間の蓄積データ
		double executionTime_[60] = {};

		// フレームカウンタ
		int timeCnt_ = 0;

		// ログ
		std::vector<std::string> vecLog_;

		// int変数情報
		std::unordered_map<const char*, int> mapTraceInt_;
		std::vector<const char*> intKeys_;

		// float変数情報
		std::unordered_map<const char*, float> mapTraceFloat_;
		std::vector<const char*> floatKeys_;

		// Vector2D変数情報
		std::unordered_map<const char*, Vector2D> mapTraceVector2D_;
		std::vector<const char*> vector2DKeys_;

		// Vector3D変数情報
		std::unordered_map<const char*, Vector3D> mapTraceVector3D_;
		std::vector<const char*> vector3DKeys_;

		// Vector4D変数情報
		std::unordered_map<const char*, Vector4D> mapTraceVector4D_;
		std::vector<const char*> vector4DKeys_;

		// リソース情報
		std::vector<std::string> vecResource_;

		// 生成の禁止
		DebugManager();
		~DebugManager(){}

		// コピーコンストラクタの禁止
		DebugManager(const DebugManager& src){}
		DebugManager& operator=(const DebugManager& src){}
	};
};

#endif	// #ifndef INCLUDE_IDEA_DEBUGMANAGER_H