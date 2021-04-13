#include "../../h/Motion/VmdMotion.h"
#include "../../h/Mesh/PmxModel.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Utility/DebugManager.h"
#include "../../h/Utility/ideaUtility.h"

#include <fstream>
#include <algorithm>
#include <array>
#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;
using namespace idea;

namespace{
	XMMATRIX Matrix4x4toXMMATRIX(const Matrix4x4& mat4x4)
	{
		XMMATRIX xmmat;

		for(int i = 0; i < 4; ++i){
			for(int j = 0; j < 4; ++j){
				xmmat.r[i].m128_f32[j] = mat4x4.r[i][j];
			}
		}

		return xmmat;
	}

	Matrix4x4 XMMATRIXtoMatrix4x4(const XMMATRIX& xmmat)
	{
		Matrix4x4 mat4x4;

		for(int i = 0; i < 4; ++i){
			for(int j = 0; j < 4; ++j){
				mat4x4.r[i][j] = xmmat.r[i].m128_f32[j];
			}
		}

		return mat4x4;
	}

	XMFLOAT3 Vector3DtoXMFLOAT3(const Vector3D& vec)
	{
		XMFLOAT3 xmf3;

		xmf3.x = vec.x;
		xmf3.y = vec.y;
		xmf3.z = vec.z;

		return xmf3;
	}

	Vector3D XMFLOAT3toVector3D(const XMFLOAT3& xmf3)
	{
		Vector3D vec;

		vec.x = xmf3.x;
		vec.y = xmf3.y;
		vec.z = xmf3.z;

		return vec;
	}

	XMFLOAT3 GetCurrentPos(const Matrix4x4& mat)
	{
		XMFLOAT3 xmf3;

		xmf3.x = mat._41;
		xmf3.y = mat._42;
		xmf3.z = mat._43;

		return xmf3;

	}
}

VmdMotion::VmdMotion() :
	motionSize_(0),
	skinSize_(0),
	cameraSize_(0),
	lightSize_(0),
	shadowSize_(0),
	ikEnableSize_(0),
	bStorage_(false),
	elapsedTime_(0.0f),
	frameMax_(0),
	bFinish_(true){}

VmdMotion::~VmdMotion()
{
	UnLoad();
}

bool VmdMotion::LoadVmdMotionFromFile(const char* pFileName)
{
	elapsedTime_ = 0.0f;

	// VMDファイルからVMDデータを抽出
	ifstream ifs(pFileName, ios::binary);
	if(!ifs.is_open()){ return false; }

	VmdHeader vmdHeader;

	//ヘッタ―の読み込み（使わない部分）
	ifs.read((char*)&vmdHeader, sizeof(VmdHeader));

	//モーションデータ-----------------------------------------------
	ifs.read((char*)&motionSize_, sizeof(motionSize_));
	vecVmdMotion_.resize(motionSize_);

	if(motionSize_ > 0){
		// モーションデータ読み込み
		ifs.read((char*)&vecVmdMotion_[0], (streamsize)sizeof(VmdMotionData) * motionSize_);

		// モーションデータ格納
		frameMax_ = 0;
		for(auto& vmdMotion : vecVmdMotion_){
			umapMotionData_[vmdMotion.boneName].emplace_back(KeyFrame(
				vmdMotion.frameNo,
				vmdMotion.quaternion,
				Vector3D(vmdMotion.location[0], vmdMotion.location[1], vmdMotion.location[2]),
				Vector2D((float)vmdMotion.bezier[3] / 127.0f, (float)vmdMotion.bezier[7] / 127.0f),
				Vector2D((float)vmdMotion.bezier[11] / 127.0f, (float)vmdMotion.bezier[15] / 127.0f)
			));
			frameMax_ = Max(frameMax_, vmdMotion.frameNo);
		}
	}

	// フレーム番号でソート
	for(auto& motion : umapMotionData_){
		if(motion.second.size() <= 1){ continue; }
		sort(motion.second.begin(), motion.second.end(),
			[](const KeyFrame& lval, const KeyFrame& rval)
			{
				return lval.frameNo < rval.frameNo;
			}
		);
	}

	// 表情データ----------------------------------------------------
	skinSize_ = 0;
	ifs.read((char*)&skinSize_, sizeof(skinSize_));
	vecVmdSkin_.resize(skinSize_);

	if(skinSize_ > 0){
		ifs.read((char*)&vecVmdSkin_[0], (streamsize)sizeof(VmdSkin) * skinSize_);
	}

	// カメラデータ--------------------------------------------------
	cameraSize_ = 0;
	ifs.read((char*)&cameraSize_, sizeof(cameraSize_));
	vecVmdCamera_.resize(cameraSize_);

	if(cameraSize_ > 0){
		ifs.read((char*)&vecVmdCamera_[0], (streamsize)sizeof(VmdCamera) * cameraSize_);
	}

	// ライト照明データ----------------------------------------------
	lightSize_ = 0;
	ifs.read((char*)&lightSize_, sizeof(lightSize_));
	vecVmdLight_.resize(lightSize_);

	if(lightSize_ > 0){
		ifs.read((char*)&vecVmdLight_[0], (streamsize)sizeof(VmdLight) * lightSize_);
	}

	// セルフ影データ------------------------------------------------
	shadowSize_ = 0;
	ifs.read((char*)&shadowSize_, sizeof(shadowSize_));
	vecVmdShadow_.resize(shadowSize_);

	if(shadowSize_ > 0){
		ifs.read((char*)&vecVmdShadow_[0], (streamsize)sizeof(VmdLight) * shadowSize_);
	}

	//IKオンオフ切り替わりデータ-------------------------------------
	ikEnableSize_ = 0;
	ifs.read((char*)&ikEnableSize_, sizeof(ikEnableSize_));
	vecIkEnable_.resize(ikEnableSize_);

	if(ikEnableSize_ > 0){
		for(auto& ikEnable : vecIkEnable_){
			//キーフレーム情報なのでまずはフレーム番号読み込み
			ifs.read((char*)&ikEnable.frameNo, sizeof(ikEnable.frameNo));

			//次に可視フラグがありますがこれは使用しないので1バイトシークでも構いません
			unsigned char visibleFlg = 0;
			ifs.read((char*)&visibleFlg, sizeof(visibleFlg));

			//対象ボーン数読み込み
			unsigned long ikBoneSize = 0;
			ifs.read((char*)&ikBoneSize, sizeof(ikBoneSize));

			//ループしつつ名前とON/OFF情報を取得
			for(int i = 0; i < (int)ikBoneSize; ++i){
				char ikBoneName[20];
				ifs.read((char*)ikBoneName, _countof(ikBoneName));
				unsigned char flg = 0;
				ifs.read((char*)&flg, sizeof(flg));
				ikEnable.ikEnableTable[ikBoneName] = flg;
			}
		}
	}

	return true;
}

bool idea::VmdMotion::LoadVmdMotionFromStorage(const char* pFileName)
{
	VmdMotion& vmd = StorageManager::Instance().GetVmdMotion(pFileName);

	//モーションデータ-----------------------------------------------
	motionSize_ = vmd.motionSize_;
	copy(vmd.vecVmdMotion_.begin(), vmd.vecVmdMotion_.end(), back_inserter(vecVmdMotion_));

	if(motionSize_ > 0){
		// モーションデータ格納
		frameMax_ = 0;
		for(auto& vmdMotion : vecVmdMotion_){
			umapMotionData_[vmdMotion.boneName].emplace_back(KeyFrame(
				vmdMotion.frameNo,
				vmdMotion.quaternion,
				Vector3D(vmdMotion.location[0], vmdMotion.location[1], vmdMotion.location[2]),
				Vector2D((float)vmdMotion.bezier[3] / 127.0f, (float)vmdMotion.bezier[7] / 127.0f),
				Vector2D((float)vmdMotion.bezier[11] / 127.0f, (float)vmdMotion.bezier[15] / 127.0f)
			));
			frameMax_ = Max(frameMax_, vmdMotion.frameNo);
		}
	}

	// フレーム番号でソート
	for(auto& motion : umapMotionData_){
		if(motion.second.size() <= 1){ continue; }
		sort(motion.second.begin(), motion.second.end(),
			[](const KeyFrame& lval, const KeyFrame& rval)
			{
				return lval.frameNo < rval.frameNo;
			}
		);
	}

	// 表情データ----------------------------------------------------
	skinSize_ = vmd.skinSize_;
	copy(vmd.vecVmdSkin_.begin(), vmd.vecVmdSkin_.end(), back_inserter(vecVmdSkin_));

	// カメラデータ--------------------------------------------------
	cameraSize_ = vmd.cameraSize_;
	copy(vmd.vecVmdCamera_.begin(), vmd.vecVmdCamera_.end(), back_inserter(vecVmdCamera_));

	// ライト照明データ----------------------------------------------
	lightSize_ = vmd.lightSize_;
	copy(vmd.vecVmdLight_.begin(), vmd.vecVmdLight_.end(), back_inserter(vecVmdLight_));

	// セルフ影データ------------------------------------------------
	shadowSize_ = vmd.shadowSize_;
	copy(vmd.vecVmdShadow_.begin(), vmd.vecVmdShadow_.end(), back_inserter(vecVmdShadow_));

	//IKオンオフ切り替わりデータ-------------------------------------
	ikEnableSize_ = vmd.ikEnableSize_;
	copy(vmd.vecIkEnable_.begin(), vmd.vecIkEnable_.end(), back_inserter(vecIkEnable_));

	bStorage_ = true;

	return true;
}

void VmdMotion::UnLoad()
{
	motionSize_ = 0;
	skinSize_ = 0;
	cameraSize_ = 0;
	lightSize_ = 0;
	shadowSize_ = 0;
	ikEnableSize_ = 0;

	elapsedTime_ = 0.0f;
	bFinish_ = true;

	vector<VmdMotionData>().swap(vecVmdMotion_);
	umapMotionData_.clear();
	vector<VmdSkin>().swap(vecVmdSkin_);
	vector<VmdCamera>().swap(vecVmdCamera_);
	vector<VmdLight>().swap(vecVmdLight_);
	vector<VmdSelfShadow>().swap(vecVmdShadow_);
	vector<VmdIKEnable>().swap(vecIkEnable_);
}

void VmdMotion::UpdateVmd(PmxModel& pmx, bool loop, float speed, bool bOfset)
{
	if(!pmx.pVertexBuffer_){ return; }

	elapsedTime_ += 0.5f * speed;
	unsigned long frameNo = static_cast<unsigned long>(elapsedTime_);

	bFinish_ = false;
	if(frameNo >= frameMax_){
		if(loop){
			elapsedTime_ = 0.0f;
			frameNo = 0;
		} else{
			bFinish_ = true;
		}
	}

	fill(pmx.vecBoneMatrix_.begin(), pmx.vecBoneMatrix_.end(), Matrix4x4Identity());

	for(auto& md : umapMotionData_){
		auto itBone = pmx.mapBone_.find(md.first);
		if(itBone == pmx.mapBone_.end()){ continue; }

		auto node = itBone->second;
		if(node.index == -1){ continue; }

		auto motions = md.second;

		auto rit = find_if(
			motions.rbegin(), motions.rend(),
			[frameNo](const KeyFrame& motion)
			{
				return motion.frameNo <= frameNo;
			}
		);

		if(rit == motions.rend()){ continue; }

		XMMATRIX rotation = XMMatrixIdentity();
		auto xmf1 = Vector3DtoXMFLOAT3(rit->offset);

		XMVECTOR offset = XMLoadFloat3(&xmf1);

		auto it = rit.base();

		if(it != motions.end()){
			float t = (elapsedTime_ - static_cast<float>(rit->frameNo))
				/ static_cast<float>(it->frameNo - rit->frameNo);

			t = Bezier(t, it->p1, it->p2, 12);

			XMVECTOR q1 = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);
			XMVECTOR q2 = XMVectorSet(it->quaternion.x, it->quaternion.y, it->quaternion.z, it->quaternion.w);
			XMVECTOR  q = XMQuaternionSlerp(q1, q2, t);

			rotation = XMMatrixRotationQuaternion(q);

			auto xmf2 = Vector3DtoXMFLOAT3(it->offset);

			offset = XMVectorLerp(offset, XMLoadFloat3(&xmf2), t);

		} else{
			XMVECTOR q = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);

			rotation = XMMatrixRotationQuaternion(q);
		}

		auto& pos = node.pos;

		if(bOfset){
			auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
				* rotation
				* XMMatrixTranslationFromVector(offset)
				* XMMatrixTranslation(pos.x, pos.y, pos.z);
			pmx.vecBoneMatrix_[node.index] = XMMATRIXtoMatrix4x4(mat);
		} else{
			auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
				* rotation
				* XMMatrixTranslation(pos.x, pos.y, pos.z);
			pmx.vecBoneMatrix_[node.index] = XMMATRIXtoMatrix4x4(mat);
		}
	}

	RecursiveMatrixMultiply(pmx, &pmx.mapBone_["センター"], Matrix4x4Identity());
}

void VmdMotion::Reset()
{
	elapsedTime_ = 0.0f;
	bFinish_ = false;
}

void VmdMotion::RecursiveMatrixMultiply(PmxModel& pmx, VmdBone* node, const Matrix4x4& mat)
{
	if(node->index == -1){ return; }

	pmx.vecBoneMatrix_[node->index] *= mat;

	for(auto& cnode : node->vecChildrenPtr){
		RecursiveMatrixMultiply(pmx, cnode, pmx.vecBoneMatrix_[node->index]);
	}
}
