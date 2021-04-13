#ifndef INCLUDE_EIDOS_VMDMOTION_H
#define INCLUDE_EIDOS_VMDMOTION_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/3DStruct.h"
#include "../../h/Utility/Timer.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaUtility.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <list>

namespace idea{
	class PmdModel;
	class PmxModel;

	class VmdMotion{
	public:
		VmdMotion();
		~VmdMotion();

		// ファイルからモーションを読み込む
		bool LoadVmdMotionFromFile(const char* pFileName);

		// ストレージからモーションを読み込む
		bool LoadVmdMotionFromStorage(const char* pFileName);

		void UnLoad();

		unsigned long GetFrameMax()const
		{
			return frameMax_;
		}

		void UpdateVmd(PmxModel& pmx, bool loop = true, float speed = 1.0f, bool bOfset = true);

		void Reset();

		bool IsFinish()const
		{
			return bFinish_;
		}

	private:
		friend class idea::PmxModel;

		struct KeyFrame{
			unsigned int frameNo;
			Quaternion quaternion;
			Vector3D offset;
			Vector2D p1;
			Vector2D p2;
			KeyFrame(unsigned int no, Quaternion q, Vector3D ofst, Vector2D ip1, Vector2D ip2)
				: frameNo(no), quaternion(q), offset(ofst), p1(ip1), p2(ip2){}
		};

		unsigned long motionSize_ = 0U;
		std::vector<VmdMotionData> vecVmdMotion_;
		std::unordered_map <std::string, std::vector<KeyFrame>> umapMotionData_;

		unsigned long skinSize_ = 0U;
		std::vector<VmdSkin> vecVmdSkin_;

		unsigned long cameraSize_ = 0U;
		std::vector<VmdCamera> vecVmdCamera_;

		unsigned long lightSize_ = 0U;
		std::vector<VmdLight> vecVmdLight_;

		unsigned long shadowSize_ = 0U;
		std::vector<VmdSelfShadow> vecVmdShadow_;

		unsigned long ikEnableSize_ = 0U;
		std::vector<VmdIKEnable> vecIkEnable_;

		// ストレージ使用フラグ
		bool bStorage_ = false;

		float elapsedTime_ = 0.0f;

		unsigned long frameMax_ = 0U;

		bool bFinish_ = false;

		void RecursiveMatrixMultiply(PmxModel& pmx, VmdBone* node, const Matrix4x4& mat);
	};
}

#endif	// #ifndef INCLUDE_EIDOS_VMDMOTION_H