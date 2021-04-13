#ifndef INCLUDE_IDEA_OBJMODEL_H
#define INCLUDE_IDEA_OBJMODEL_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../3D/3DStruct.h"
#include "../Texture/Texture.h"
#include "../Utility/ideaMath.h"
#include "../Utility/ideaUtility.h"
#include <vector>

struct ID3D11ShaderResourceView;
struct ID3D11Buffer;

namespace idea{
	class ObjModel : public Object{
	public:
		ObjModel();
		~ObjModel();

		//objメッシュをファイルから読み込む
		bool LoadObjMeshFromFile(const char* pFileName);

		//objメッシュをアーカイブファイルから読み込む
		bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// objメッシュをストレージから読み込む
		bool LoadObjMeshFromStorage(const char* pFileName);

		// メッシュの破棄
		void UnLoad();

	private:
		//頂点バッファ
		ID3D11Buffer* pVertexBuffer_ = nullptr;

		//インデックスバッファ
		ID3D11Buffer* pIndexBuffer_ = nullptr;

		// 頂点情報リスト
		std::vector<MeshVertexData> vecVertex_;

		// 頂点数
		unsigned int vertexSize_ = 0U;

		// インデックスリスト
		std::vector<unsigned short> vecIndex_;

		// インデックス数
		unsigned int indexSize_ = 0U;

		// サブセットリスト
		std::vector<ObjSubset> vecSubset_;

		// サブセット数
		unsigned int subsetSize_ = 0U;

		// マテリアルリスト
		std::vector<ObjMaterial> vecMaterial_;

		// マテリアル数
		unsigned int materialSize_ = 0U;

		// テクスチャーポインターリスト
		std::vector<idea::Texture*> vecTexPtr_;

		// テクスチャー数
		unsigned int texPtrSize_ = 0U;

		// ストレージ使用フラグ
		bool bStorage_ = false;

		//マテリアルファイルをファイルから読み込む
		inline bool LoadMaterialFromFile(const char* pFileName);

		//マテリアルファイルをアーカイブファイルから読み込む
		inline bool LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// 描画
		void Draw(Camera* pCamera)override;

		// コピーコンストラクタの禁止
		ObjModel(const ObjModel& src){}
		ObjModel& operator=(const ObjModel& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_OBJMODEL_H
