#ifndef INCLUDE_IDEA_3DSTRUCT_H
#define INCLUDE_IDEA_3DSTRUCT_H

#include "../Texture/Texture.h"
#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace idea{
	// メッシュの頂点データ
	struct MeshVertexData{
		Vector3D pos;
		Vector3D nor;
		Color color;
		Vector2D tex;
	};

	// メッシュの頂点データ
	struct BillboardVertexData{
		Vector3D pos;
		Vector2D tex;
	};

	// ブレンド重み付メッシュの頂点データ
	struct BlendVertexData{
		Vector3D pos;
		Vector3D nor;
		Color col;
		Vector2D tex;
		float weight[3];
		unsigned int boneIndex[4];
	};

	// マテリアル構造体
	struct Material{
		Color ambient;
		Color diffuse;
		Color specular;
		Color emission;
		float shininess;
	};

#pragma pack(push,1)
	// .obj
	// サブセット
	struct ObjSubset{
		unsigned int materialIndex;
		unsigned int faceStart;
		unsigned int faceCount;
	};

	// マテリアル構造体
	struct ObjMaterial{
		char name[256];
		Vector3D ambient;
		Vector3D diffuse;
		Vector3D specular;
		float shininess;
		float alpha;
		char ambientMapName[256];
		char diffuseMapName[256];
		char specularMapName[256];
		char bumpMapName[256];
	};

#pragma pack(pop)

	struct IKLink{
		int index;
		std::string name;
		bool existAngleLimited;
		Vector3D limitAngleMin;
		Vector3D limitAngleMax;
	};

	// .pmx
	// PMXのモデルデータ型
	struct PMXModelData{
		static constexpr int NO_DATA_FLAG = -1;

		// 頂点情報
		struct Vertex{
			Vector3D position;
			Vector3D normal;
			Vector2D uv;
			std::vector<Vector4D> additionalUV;

			// ボーンウェイト
			struct Weight{
				enum Type{
					BDEF1,
					BDEF2,
					BDEF4,
					SDEF,
				};

				Type type;

				int born1;
				int	born2;
				int	born3;
				int	born4;

				float weight1;
				float weight2;
				float weight3;
				float weight4;

				Vector3D c;
				Vector3D r0;
				Vector3D r1;
			} weight;

			// エッジ倍率
			float edgeMagnif;
		};

		struct Surface{
			unsigned short vertexIndex;
		};

		struct Material{
			Vector4D diffuse;
			Vector3D specular;
			float specularity;
			Vector3D ambient;

			int colorMapTextureIndex;
			int toonTextureIndex;

			int vertexNum;
		};

		// ボーン情報
		struct Bone{
			std::string name;

			std::string nameEnglish;

			Vector3D position;

			int parentIndex;

			int transformationLevel;

			unsigned short flag;

			Vector3D coordOffset;

			int childrenIndex;
			int impartParentIndex;
			float impartRate;

			Vector3D fixedAxis;

			Vector3D localAxisX;

			Vector3D localAxisZ;

			int externalParentKey;

			int ikTargetIndex;
			int ikLoopCount;
			float ikUnitAngle;

			std::vector<IKLink> ikLinks;
		};

		std::vector<Vertex> vertices;
		std::vector<Surface> surfaces;
		std::vector<std::string> texturePaths;
		std::vector<Material> materials;
		std::vector<Bone> bones;
	};

#pragma pack(push,1)

	// .vmd
	// VMDのヘッダー
	struct VmdHeader{
		// マジックナンバー"Vocaloid Motion Data 0002"
		char vmdHeader[30];
		
		// カメラの場合:"カメラ・照明"
		char vmdModelName[20];
	};

	// モーション
	struct VmdMotionData{
		// ボーン名
		char boneName[15];

		// フレーム番号
		unsigned long frameNo;
		
		// 移動量
		float location[3];

		// 回転量 (モデルローカル座標系
		Quaternion quaternion;
		
		// 補完
		unsigned char bezier[64];
	};

	// 表情データ
	struct VmdSkin{
		// 表情名
		char skinName[15];
		
		// フレーム番号
		unsigned long flameNo;
		
		// 表情の設定値(表情スライダーの値)
		float weight;
	};

	// カメラ
	struct VmdCamera{
		// フレーム番号
		unsigned long frameNo;
		
		// 距離
		float distance;
		
		// 座標
		Vector3D pos;
		
		// オイラー角
		Vector3D eulerAngle;
		
		// 補完
		unsigned char interpolation[24];

		// 視界角
		unsigned long fov;
		
		// パースフラグON/OFF
		unsigned char persFlg;
	};

	// ライト照明データ
	struct VmdLight{
		
		// フレーム番号
		unsigned long frameNo;
		
		// ライト色
		Vector3D rgb;
		
		// 光線ベクトル(平行光線)
		Vector3D vec;
	};

	// セルフ影データ
	struct VmdSelfShadow{
		// フレーム番号
		unsigned long frameNo;
		
		//影モード(0:影なし、1:モード１、2:モード２)
		unsigned char mode;
		
		//距離
		float distance;
	};

	// IKオンオフデータ
	struct VmdIKEnable {
		// フレーム番号
		unsigned long frameNo;

		// 有効情報
		std::unordered_map<std::string, bool> ikEnableTable;
	};

#pragma pack(pop)

	// ボーン
	struct VmdBone{
		VmdBone() : type(0), index(-1){}

		// ボーンID（通し番号）
		long index = -1;

		// ボーン名
		std::string name;

		// ボーンタイプ (MMDの場合 0:回転 1:回転と移動 2:IK 3:不明 4:IK影響下 5:回転影響下 6:IK接続先 7:非表示 8:捻り 9:回転運動 )
		unsigned char type = 0;

		// 座標
		Vector3D pos;

		// 親ボーン番号
		long parentIndex = -1;

		// 親ボーン名
		std::string parentName;

		// IK親ボーン番号
		long ikParentBone = -1;

		// IK親ボーン名
		std::string ikParentName;

		// IKターゲットのボーン番号
		long ikTargetIndex = -1;

		// IKターゲットのボーン名
		std::string ikTargetName;

		// 試行回数
		int ikLoopCount = 0;

		// IKの角度
		float ikUnitAngle = 0.0f;

		// IK接続リスト
		std::vector<IKLink> ikLinks;

		// IK接続リスト数
		int ikLinkSize = 0;

		// 子ボーンリスト
		std::vector<VmdBone*> vecChildrenPtr;
	};

	// IK データ
	struct VmdIkData{
		// IK対象のボーン番号
		unsigned int boneIdx;

		// IK対象のボーン名
		std::string name;

		// ターゲットに近づけるためのボーン番号
		unsigned int targetIdx;

		// ターゲットに近づけるためのボーン名
		std::string targetName;
		
		// 試行回数
		unsigned int loopCount;

		// 一回当たりの回転制限
		float limit;
		
		// 間のノード番号
		std::vector<int> nodeIdxes;
		
		// 間のノード番号
		std::vector<std::string> nodeNames;

		// 間のノード数
		unsigned int nodeSize;
	};
}

#endif	// #ifndef INCLUDE_IDEA_3DSTRUCT_H