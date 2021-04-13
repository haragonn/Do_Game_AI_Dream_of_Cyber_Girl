#ifndef INCLUDE_EIDOS_PMXMODEL_H
#define INCLUDE_EIDOS_PMXMODEL_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../3D/3DStruct.h"
#include "../Motion/VmdMotion.h"
#include "../Texture/Texture.h"
#include "../Utility/ideaMath.h"

struct ID3D11Buffer;

namespace idea{
	class PmxModel : public Object{
	public:
		PmxModel();
		~PmxModel();

		// ファイルからメッシュを読み込む
		bool LoadPmxMeshFromFile(const char* pFileName);

		// アーカイブファイルからメッシュを読み込む
		bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// ストレージからメッシュを読み込む
		bool LoadPmxMeshFromStorage(const char* pFileName);

		// メッシュの破棄
		void UnLoad();

		void UpdateVmd(idea::VmdMotion& vmd, float speed = 1.0f, float blendSpeed = 0.1f, bool loop = true, bool bOfset = true);

		bool IsFinishMotion()const
		{
			return bFinish_;
		}

	private:
		friend class idea::VmdMotion;

		//頂点バッファ
		ID3D11Buffer* pVertexBuffer_ = nullptr;
		
		//インデックスバッファ
		ID3D11Buffer* pIndexBuffer_ = nullptr;

		// 頂点サイズ
		unsigned long vertexSize_ = 0U;

		// 頂点情報
		std::vector<BlendVertexData> vecVertex_;

		// インデックスサイズ
		unsigned long indexSize_ = 0U;

		// インデックス情報
		std::vector<unsigned short> vecIndex_;

		// マテリアルサイズ
		unsigned long materialSize_ = 0U;

		// マテリアル情報
		std::vector<idea::PMXModelData::Material> vecMaterial_;

		// テクスチャポインタサイズ
		unsigned long texPtrSize_ = 0U;

		// テクスチャポインタ
		std::vector<idea::Texture*> vecTexPtr_;

		// ボーンサイズ
		unsigned short boneSize_ = 0U;

		// ボーン情報
		std::vector<VmdBone> vecBone_;
		std::unordered_map<std::string, VmdBone> mapBone_;

		// ボーンマトリクス
		std::vector<idea::Matrix4x4> vecBoneMatrix_;

		// IK情報サイズ
		unsigned short ikSize_ = 0U;

		// IK情報
		std::vector<idea::VmdIkData> vecIk_;

		// ひざインデックスサイズ
		unsigned long kneeIndexSize_ = 0U;

		// ひざインデックス
		std::vector<unsigned long> vecKneeIndex;

		// ストレージ使用フラグ
		bool bStorage_ = false;

		float elapsedTime_ = 0.0f;

		bool bBlend_ = false;

		bool bFinish_ = false;

		float blendRate_ = 0.0f;

		idea::VmdMotion* pPreMotion_ = nullptr;

		unsigned long preLastFranme_ = 0U;

		float t_ = 0.0f;

		//描画
		void Draw(idea::Camera* pCamera)override;

		void RecursiveMatrixMultiply(VmdBone* node, const idea::Matrix4x4& mat);

		// コピーコンストラクタの禁止
		PmxModel(const idea::PmxModel& src){}
		PmxModel& operator=(const idea::PmxModel& src){}
	};
}
#endif	// #ifndef INCLUDE_EIDOS_PMXMODEL_H
