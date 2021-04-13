#include "../../h/Mesh/PmxModel.h"
#include "../../h/Storage/StorageManager.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/3D/ObjectManager.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Archive/ArchiveLoader.h"
#include "../../h/Utility/DebugManager.h"
#include "../../h/Utility/ideaUtility.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <array>

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <DirectXMath.h>

using namespace std;
using namespace DirectX;
using namespace idea;

namespace{
	XMFLOAT3 Vector3DtoXMFLOAT3(const Vector3D& vec)
	{
		XMFLOAT3 xmf3;

		xmf3.x = vec.x;
		xmf3.y = vec.y;
		xmf3.z = vec.z;

		return xmf3;
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

	XMFLOAT4X4 Matrix4x4toXMFLOAT4X4(const Matrix4x4& mat4x4)
	{
		XMFLOAT4X4 xmf;

		for(int i = 0; i < 4; ++i){
			for(int j = 0; j < 4; ++j){
				xmf.m[i][j] = mat4x4.r[i][j];
			}
		}

		return xmf;
	}

	bool GetPMXStringUTF16(std::ifstream& file, std::wstring& output)
	{
		std::array<wchar_t, 512> wBuffer{};
		int textSize;

		file.read(reinterpret_cast<char*>(&textSize), 4);

		file.read(reinterpret_cast<char*>(&wBuffer), textSize);
		output = std::wstring(&wBuffer[0], &wBuffer[0] + textSize / 2);

		return true;
	}

	bool GetPMXStringUTF16S(std::stringstream& file, std::wstring& output)
	{
		std::array<wchar_t, 512> wBuffer{};
		int textSize;

		file.read(reinterpret_cast<char*>(&textSize), 4);

		file.read(reinterpret_cast<char*>(&wBuffer), textSize);
		output = std::wstring(&wBuffer[0], &wBuffer[0] + textSize / 2);

		return true;
	}
}

PmxModel::PmxModel() :
	pVertexBuffer_(nullptr),
	pIndexBuffer_(nullptr),
	vertexSize_(0),
	indexSize_(0),
	materialSize_(0),
	boneSize_(0),
	ikSize_(0),
	texPtrSize_(0),
	bStorage_(false),
	elapsedTime_(0.0f),
	bBlend_(false),
	bFinish_(false),
	blendRate_(0.0f),
	pPreMotion_(nullptr){}

PmxModel::~PmxModel()
{
	UnLoad();
}

bool PmxModel::LoadPmxMeshFromFile(const char* pFileName)
{
	if(pVertexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }

	ifstream ifs(pFileName, ios::binary);
	if(!ifs.is_open()){ return false; }

	string filePath = pFileName;

	string folderPath{ filePath.begin(), filePath.begin() + filePath.rfind('/') + 1 };

	// ヘッダー -------------------------------
	array<byte, 4> pmxHeader = {};
	constexpr array<byte, 4> PMX_MAGIC_NUMBER{ 0x50, 0x4d, 0x58, 0x20 };
	enum HeaderDataIndex
	{
		ENCODING_FORMAT,
		NUMBER_OF_ADD_UV,
		VERTEX_INDEX_SIZE,
		TEXTURE_INDEX_SIZE,
		MATERIAL_INDEX_SIZE,
		BONE_INDEX_SIZE,
		RIGID_BODY_INDEX_SIZE
	};

	for(int i = 0; i < 4; ++i){
		pmxHeader[i] = ifs.get();
	}
	if(pmxHeader != PMX_MAGIC_NUMBER){
		ifs.close();
		return false;
	}

	// ver2.0以外は非対応
	float version = 0.0f;
	ifs.read(reinterpret_cast<char*>(&version), 4);
	if(!XMScalarNearEqual(version, 2.0f, g_XMEpsilon.f[0])){
		ifs.close();
		return false;
	}

	byte hederDataLength = ifs.get();
	if(hederDataLength != 8){
		ifs.close();
		return false;
	}

	array<byte, 8> hederData{};
	for(int i = 0; i < hederDataLength; ++i){
		hederData[i] = ifs.get();
	}

	//UTF-8は非対応
	if(hederData[0] != 0){
		ifs.close();
		return false;
	}

	unsigned int arrayLength = 0;

	for(int i = 0; i < 4; ++i){
		ifs.read(reinterpret_cast<char*>(&arrayLength), 4);

		for(unsigned int j = 0; j < arrayLength; ++j){
			ifs.get();
		}
	}

	PMXModelData pmxData_;;
	vector<PMXModelData::Vertex>().swap(pmxData_.vertices);
	vector<PMXModelData::Surface>().swap(pmxData_.surfaces);
	vector<string>().swap(pmxData_.texturePaths);
	vector<PMXModelData::Material>().swap(pmxData_.materials);
	vector<PMXModelData::Bone>().swap(pmxData_.bones);

	// 頂点 -----------------------------------
	using Vertex = PMXModelData::Vertex;
	int numberOfVertex = 0;
	ifs.read(reinterpret_cast<char*>(&numberOfVertex), 4);
	vertexSize_ = numberOfVertex;
	pmxData_.vertices.resize(numberOfVertex);

	for(int i = 0; i < numberOfVertex; ++i)
	{
		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].position), 12);
		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].normal), 12);
		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].uv), 8);

		if(hederData[NUMBER_OF_ADD_UV] != 0){
			for(int j = 0; j < hederData[NUMBER_OF_ADD_UV]; ++j){
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].additionalUV[j]), 16);
			}
		}

		const byte weightMethod = ifs.get();

		switch(weightMethod){
			case Vertex::Weight::BDEF1:
				pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF1;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				pmxData_.vertices[i].weight.born2 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.weight1 = 1.0f;
				break;

			case Vertex::Weight::BDEF2:
				pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF2;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
				pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
				pmxData_.vertices[i].weight.weight2 = 1.0f - pmxData_.vertices[i].weight.weight1;
				break;

			case Vertex::Weight::BDEF4:
				pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF4;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born3), hederData[BONE_INDEX_SIZE]);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born4), hederData[BONE_INDEX_SIZE]);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight2), 4);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight3), 4);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight4), 4);
				break;

			case Vertex::Weight::SDEF:
				pmxData_.vertices[i].weight.type = Vertex::Weight::SDEF;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
				pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
				pmxData_.vertices[i].weight.weight2 = 1.0f - pmxData_.vertices[i].weight.weight1;
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.c), 12);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.r0), 12);
				ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.r1), 12);
				break;

			default:
				ifs.close();
				return false;
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.vertices[i].edgeMagnif), 4);

		if(pmxData_.vertices[i].weight.born1 == PMXModelData::NO_DATA_FLAG){
			ifs.close();
			return false;
		}
	}

	// 面  ------------------------------------
	int numOfSurface = 0;
	ifs.read(reinterpret_cast<char*>(&numOfSurface), 4);
	pmxData_.surfaces.resize(numOfSurface);
	indexSize_ = numOfSurface;

	for(int i = 0; i < numOfSurface; ++i){
		ifs.read(reinterpret_cast<char*>(&pmxData_.surfaces[i].vertexIndex), hederData[VERTEX_INDEX_SIZE]);

		if(pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG || pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG || pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG){
			ifs.close();
			return false;
		}
	}

	// テクスチャ -----------------------------
	int numOfTexture = 0;
	ifs.read(reinterpret_cast<char*>(&numOfTexture), 4);

	texPtrSize_ = numOfTexture;

	pmxData_.texturePaths.resize(numOfTexture);

	// テクスチャ読み込み
	wstring texturePath{};
	for(int i = 0; i < numOfTexture; ++i){
		GetPMXStringUTF16(ifs, texturePath);

		string t = WStringToString(texturePath);
		string s = folderPath;
		AddDirectoryPath(t, s);

		pmxData_.texturePaths[i] = t;

		Texture* pTex = new Texture;
		pTex->LoadImageFromFile(t.c_str());
		vecTexPtr_.push_back(pTex);
	}

	// マテリアル -----------------------------
	int numOfMaterial = 0;
	ifs.read(reinterpret_cast<char*>(&numOfMaterial), 4);

	materialSize_ = numOfMaterial;

	pmxData_.materials.resize(numOfMaterial);

	for(int i = 0; i < numOfMaterial; ++i){
		for(int j = 0; j < 2; ++j){
			ifs.read(reinterpret_cast<char*>(&arrayLength), 4);
			for(unsigned int k = 0; k < arrayLength; ++k){ ifs.get(); }
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].diffuse), 16);
		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].specular), 12);
		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].specularity), 4);
		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].ambient), 12);

		ifs.get();

		for(int j = 0; j < 16; ++j){ ifs.get(); }

		for(int j = 0; j < 4; ++j){ ifs.get(); }

		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].colorMapTextureIndex), hederData[TEXTURE_INDEX_SIZE]);

		for(unsigned char j = 0; j < hederData[TEXTURE_INDEX_SIZE]; ++j){ ifs.get(); }

		ifs.get();

		const byte shareToonFlag = ifs.get();

		if(shareToonFlag){
			ifs.get();
		} else{
			ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].toonTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		}

		ifs.read(reinterpret_cast<char*>(&arrayLength), 4);

		for(unsigned int j = 0; j < arrayLength; ++j){ ifs.get(); }

		ifs.read(reinterpret_cast<char*>(&pmxData_.materials[i].vertexNum), 4);
	}

	// ボーン ---------------------------------
	enum BoneFlagMask{
		ACCESS_POINT = 0x0001,
		IK = 0x0020,
		IMPART_TRANSLATION = 0x0100,
		IMPART_ROTATION = 0x0200,
		AXIS_FIXING = 0x0400,
		LOCAL_AXIS = 0x0800,
		EXTERNAL_PARENT_TRANS = 0x2000,
	};
	int numOfBone = 0;
	ifs.read(reinterpret_cast<char*>(&numOfBone), 4);

	boneSize_ = numOfBone;

	pmxData_.bones.resize(numOfBone);

	int ikLinkSize = 0;
	unsigned char angleLim = 0;

	for(int i = 0; i < numOfBone; ++i){
		wstring buf;
		GetPMXStringUTF16(ifs, buf);
		pmxData_.bones[i].name = WStringToString(buf);

		ifs.read(reinterpret_cast<char*>(&arrayLength), 4);
		pmxData_.bones[i].nameEnglish.resize(arrayLength);

		for(unsigned int j = 0; j < arrayLength; ++j){
			pmxData_.bones[i].nameEnglish[j] = ifs.get();
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].position), 12);

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].parentIndex), hederData[BONE_INDEX_SIZE]);

		if(numOfBone <= pmxData_.bones[i].parentIndex){
			pmxData_.bones[i].parentIndex = PMXModelData::NO_DATA_FLAG;
		}

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].transformationLevel), 4);

		ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].flag), 2);

		if(pmxData_.bones[i].flag & ACCESS_POINT){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].childrenIndex), hederData[BONE_INDEX_SIZE]);
			if(numOfBone <= pmxData_.bones[i].childrenIndex)
			{
				pmxData_.bones[i].childrenIndex = PMXModelData::NO_DATA_FLAG;
			}
		} else{
			pmxData_.bones[i].childrenIndex = PMXModelData::NO_DATA_FLAG;
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].coordOffset), 12);
		}

		if((pmxData_.bones[i].flag & IMPART_TRANSLATION) || (pmxData_.bones[i].flag & IMPART_ROTATION)){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].impartParentIndex), hederData[BONE_INDEX_SIZE]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].impartRate), 4);
		}

		if(pmxData_.bones[i].flag & AXIS_FIXING){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].fixedAxis), 12);
		}

		if(pmxData_.bones[i].flag & LOCAL_AXIS){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].localAxisX), 12);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].localAxisZ), 12);
		}

		if(pmxData_.bones[i].flag & EXTERNAL_PARENT_TRANS){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].externalParentKey), 4);
		}

		if(pmxData_.bones[i].flag & IK){
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikTargetIndex), hederData[5]);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLoopCount), 4);
			ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikUnitAngle), 4);
			ifs.read(reinterpret_cast<char*>(&ikLinkSize), 4);

			pmxData_.bones[i].ikLinks.resize(ikLinkSize);

			for(int j = 0; j < ikLinkSize; ++j){
				ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].index), hederData[5]);
				angleLim = ifs.get();

				pmxData_.bones[i].ikLinks[j].existAngleLimited = false;

				if(angleLim == 1){
					ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].limitAngleMin), 12);
					ifs.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].limitAngleMax), 12);
					pmxData_.bones[i].ikLinks[j].existAngleLimited = true;
				}
			}
		} else{
			pmxData_.bones[i].ikTargetIndex = PMXModelData::NO_DATA_FLAG;
		}
	}
	for(unsigned int i = 0; i < boneSize_; ++i){
		for(auto& ikl : pmxData_.bones[i].ikLinks){
			if(ikl.index != PMXModelData::NO_DATA_FLAG){
				ikl.name = pmxData_.bones[ikl.index].name;
			}
		}
	}

	ifs.close();

	vecVertex_.resize(vertexSize_);

	for(unsigned int i = 0; i < vertexSize_; ++i){
		vecVertex_[i].pos.x = pmxData_.vertices[i].position.x;
		vecVertex_[i].pos.y = pmxData_.vertices[i].position.y;
		vecVertex_[i].pos.z = pmxData_.vertices[i].position.z;

		vecVertex_[i].nor.x = pmxData_.vertices[i].normal.x;
		vecVertex_[i].nor.y = pmxData_.vertices[i].normal.y;
		vecVertex_[i].nor.z = pmxData_.vertices[i].normal.z;

		vecVertex_[i].tex.x = pmxData_.vertices[i].uv.x;
		vecVertex_[i].tex.y = pmxData_.vertices[i].uv.y;

		vecVertex_[i].col = ideaColor::WHITE;

		vecVertex_[i].boneIndex[0] = pmxData_.vertices[i].weight.born1;
		vecVertex_[i].boneIndex[1] = pmxData_.vertices[i].weight.born2;
		vecVertex_[i].boneIndex[2] = pmxData_.vertices[i].weight.born3;
		vecVertex_[i].boneIndex[3] = pmxData_.vertices[i].weight.born4;

		vecVertex_[i].weight[0] = pmxData_.vertices[i].weight.weight1;
		vecVertex_[i].weight[1] = pmxData_.vertices[i].weight.weight2;
		vecVertex_[i].weight[2] = pmxData_.vertices[i].weight.weight3;
	}

	vecIndex_.resize(indexSize_);

	for(unsigned int i = 0; i < indexSize_; ++i){
		vecIndex_[i] = pmxData_.surfaces[i].vertexIndex;
	}

	vecMaterial_.resize(materialSize_);

	for(unsigned int i = 0; i < materialSize_; ++i){
		vecMaterial_[i].diffuse = pmxData_.materials[i].diffuse;
		vecMaterial_[i].specular = pmxData_.materials[i].specular;
		vecMaterial_[i].specularity = pmxData_.materials[i].specularity;
		vecMaterial_[i].ambient = pmxData_.materials[i].ambient;
		vecMaterial_[i].colorMapTextureIndex = pmxData_.materials[i].colorMapTextureIndex;
		vecMaterial_[i].vertexNum = pmxData_.materials[i].vertexNum;
	}

	// ボーン情報格納
	vecBone_.resize(boneSize_);
	vector<string> vecBoneName(boneSize_);

	for(unsigned int i = 0; i < boneSize_; ++i){
		auto& pd = pmxData_.bones[i];

		vecBoneName[i] = pd.name;
		auto& node = mapBone_[pd.name];
		node.index = i;
		node.name = pd.name;

		if(node.name.find("ひざ") != std::string::npos) {
			vecKneeIndex.emplace_back(i);
		}

		node.parentIndex = pd.parentIndex;
		if(pd.parentIndex != -1){
			node.parentName = pmxData_.bones[pd.parentIndex].name;
		} else{
			node.parentName = "";
		}

		if(pd.flag & IK){
			node.type = 3;

			node.ikTargetIndex = pd.ikTargetIndex;
			if(pd.ikTargetIndex != -1){
				node.ikTargetName = pmxData_.bones[pd.ikTargetIndex].name;
			} else{
				node.ikTargetName = "";
			}
			node.ikLoopCount = pd.ikLoopCount;
			node.ikUnitAngle = pd.ikUnitAngle;
			copy(pd.ikLinks.begin(), pd.ikLinks.end(), back_inserter(node.ikLinks));
			node.ikLinkSize = (int)pd.ikLinks.size();

			VmdIkData ik;

			ik.boneIdx = node.index;
			ik.name = node.name;
			ik.targetIdx = node.ikTargetIndex;
			ik.targetName = node.ikTargetName;
			ik.loopCount = node.ikLoopCount;
			ik.limit = node.ikUnitAngle;
			for(int i = 0; i < node.ikLinkSize; ++i){
				ik.nodeIdxes.push_back(node.ikLinks[i].index);
				ik.nodeNames.push_back(node.ikLinks[i].name);
			}
			ik.nodeSize = ik.nodeIdxes.size();

			vecIk_.push_back(ik);
		} else{
			node.type = 0;

			node.ikTargetIndex = -1;
			node.ikTargetName = "";
			node.ikLoopCount = 0;
			node.ikUnitAngle = 0.0f;
			node.ikLinkSize = 0;
		}

		node.pos = pd.position;

		node.ikParentBone = -1;
		node.ikParentName = "";

		vecBone_[i] = node;
	}

	kneeIndexSize_ = (unsigned long)vecKneeIndex.size();

	ikSize_ = (unsigned short)vecIk_.size();

	for(int i = 0; i < (int)ikSize_; ++i){
		for(int j = 0; j < (int)vecIk_[i].nodeSize; ++j){
			vecBone_[vecIk_[i].nodeIdxes[j]].ikParentBone = vecIk_[i].boneIdx;
			vecBone_[vecIk_[i].nodeIdxes[j]].ikParentName = vecIk_[i].name;
		}
	}

	for(unsigned int i = 0; i < boneSize_; ++i){
		mapBone_[vecBoneName[i]] = vecBone_[i];
	}

	for(auto& pd : pmxData_.bones){
		if(pd.parentIndex >= boneSize_ || pd.parentIndex == PMXModelData::NO_DATA_FLAG){
			continue;
		}
		auto parentName = vecBoneName[pd.parentIndex];
		mapBone_[parentName].vecChildrenPtr.emplace_back(&mapBone_[pd.name]);
		vecBone_[mapBone_[parentName].index].vecChildrenPtr.emplace_back(&mapBone_[pd.name]);
	}

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4Identity());

	{
		// 頂点バッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(BlendVertexData) * vertexSize_;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecVertex_[0];
		if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pVertexBuffer_))){ return false; }
	}

	{
		// インデックスバッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(unsigned short) * indexSize_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecIndex_[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pIndexBuffer_))){ return false; }

	}

	DebugManager::Instance().AddResource(pFileName);

	return true;
}

bool PmxModel::LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName)
{
	if(pVertexBuffer_){ return false; }	// 既に読み込み済みなら終了

	GraphicManager& gm = GraphicManager::Instance();
	if(!gm.GetContextPtr()){ return false; }
	if(!gm.GetContextPtr()){ return false; }

	// tmpファイル名作成
	char tmpP[] = "p";
	char tempFileData[] = "pmx_";
	char tmpM[] = "m";
	char tempFileNum[] = "000";
	char tmpT[] = "t";
	char tempFileName[256] = "eidos_";
	char tmpDot[] = ".";
	strcat_s(tempFileName, tempFileData);
	strcat_s(tempFileName, tempFileNum);
	strcat_s(tempFileName, tmpDot);
	strcat_s(tempFileName, tmpT);
	strcat_s(tempFileName, tmpM);
	strcat_s(tempFileName, tmpP);

	// アーカイブファイルを開く
	ArchiveLoader loader;
	if(!loader.Load(pArchiveFileName, pFileName)){
		SetDebugMessage("PmxMeshLoadError! [%s] を [%s] から読み込めませんでした。\n", pFileName, pArchiveFileName);
		return false;
	}

	// tmpファイル書き込み
	FILE* fp;

	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite(&loader.GetData()[0], loader.GetSize(), sizeof(BYTE), fp);
		fclose(fp);
	}

	// ファイルを開いて一時ストリームに書き込む
	ifstream file;
	file.open(tempFileName, ios::binary);
	if(!file.is_open()){ return false; }

	stringstream tmpStream;
	tmpStream << file.rdbuf();

	file.close();

	// tmpファイルを念のため初期化
	if(fopen_s(&fp, tempFileName, "wb") != 0){ return false; }

	if(fp == NULL) {
		return false;
	} else {
		fwrite("", 1, sizeof(BYTE), fp);
		fclose(fp);
	}

	DeleteFileA(tempFileName);	//tmpファイル削除
	string filePath = pFileName;

	string folderPath;// { filePath.begin(), filePath.begin() + filePath.rfind('/') + 1 };

	// ヘッダー -------------------------------
	array<byte, 4> pmxHeader = {};
	constexpr array<byte, 4> PMX_MAGIC_NUMBER{ 0x50, 0x4d, 0x58, 0x20 };
	enum HeaderDataIndex
	{
		ENCODING_FORMAT,
		NUMBER_OF_ADD_UV,
		VERTEX_INDEX_SIZE,
		TEXTURE_INDEX_SIZE,
		MATERIAL_INDEX_SIZE,
		BONE_INDEX_SIZE,
		RIGID_BODY_INDEX_SIZE
	};

	for(int i = 0; i < 4; ++i){
		pmxHeader[i] = tmpStream.get();
	}
	if(pmxHeader != PMX_MAGIC_NUMBER){
		return false;
	}

	// ver2.0以外は非対応
	float version = 0.0f;
	tmpStream.read(reinterpret_cast<char*>(&version), 4);
	if(!XMScalarNearEqual(version, 2.0f, g_XMEpsilon.f[0])){
		return false;
	}

	byte hederDataLength = tmpStream.get();
	if(hederDataLength != 8){
		return false;
	}

	array<byte, 8> hederData{};
	for(int i = 0; i < hederDataLength; ++i){
		hederData[i] = tmpStream.get();
	}

	//UTF-8は非対応
	if(hederData[0] != 0){
		return false;
	}

	unsigned int arrayLength = 0;

	for(int i = 0; i < 4; ++i){
		tmpStream.read(reinterpret_cast<char*>(&arrayLength), 4);

		for(unsigned int j = 0; j < arrayLength; ++j){
			tmpStream.get();
		}
	}

	PMXModelData pmxData_;;
	vector<PMXModelData::Vertex>().swap(pmxData_.vertices);
	vector<PMXModelData::Surface>().swap(pmxData_.surfaces);
	vector<string>().swap(pmxData_.texturePaths);
	vector<PMXModelData::Material>().swap(pmxData_.materials);
	vector<PMXModelData::Bone>().swap(pmxData_.bones);

	// 頂点 -----------------------------------
	using Vertex = PMXModelData::Vertex;
	int numberOfVertex = 0;
	tmpStream.read(reinterpret_cast<char*>(&numberOfVertex), 4);
	vertexSize_ = numberOfVertex;
	pmxData_.vertices.resize(numberOfVertex);

	for(int i = 0; i < numberOfVertex; ++i)
	{
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].position), 12);
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].normal), 12);
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].uv), 8);

		if(hederData[NUMBER_OF_ADD_UV] != 0){
			for(int j = 0; j < hederData[NUMBER_OF_ADD_UV]; ++j){
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].additionalUV[j]), 16);
			}
		}

		const byte weightMethod = tmpStream.get();

		switch(weightMethod){
			case Vertex::Weight::BDEF1:
				pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF1;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				pmxData_.vertices[i].weight.born2 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.weight1 = 1.0f;
				break;

			case Vertex::Weight::BDEF2:
				pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF2;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
				pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
				pmxData_.vertices[i].weight.weight2 = 1.0f - pmxData_.vertices[i].weight.weight1;
				break;

			case Vertex::Weight::BDEF4:
				pmxData_.vertices[i].weight.type = Vertex::Weight::BDEF4;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born3), hederData[BONE_INDEX_SIZE]);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born4), hederData[BONE_INDEX_SIZE]);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight2), 4);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight3), 4);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight4), 4);
				break;

			case Vertex::Weight::SDEF:
				pmxData_.vertices[i].weight.type = Vertex::Weight::SDEF;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born1), hederData[BONE_INDEX_SIZE]);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.born2), hederData[BONE_INDEX_SIZE]);
				pmxData_.vertices[i].weight.born3 = PMXModelData::NO_DATA_FLAG;
				pmxData_.vertices[i].weight.born4 = PMXModelData::NO_DATA_FLAG;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.weight1), 4);
				pmxData_.vertices[i].weight.weight2 = 1.0f - pmxData_.vertices[i].weight.weight1;
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.c), 12);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.r0), 12);
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].weight.r1), 12);
				break;

			default:
				return false;
		}

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.vertices[i].edgeMagnif), 4);

		if(pmxData_.vertices[i].weight.born1 == PMXModelData::NO_DATA_FLAG){
			return false;
		}
	}

	// 面  ------------------------------------
	int numOfSurface = 0;
	tmpStream.read(reinterpret_cast<char*>(&numOfSurface), 4);
	pmxData_.surfaces.resize(numOfSurface);
	indexSize_ = numOfSurface;

	for(int i = 0; i < numOfSurface; ++i){
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.surfaces[i].vertexIndex), hederData[VERTEX_INDEX_SIZE]);

		if(pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG || pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG || pmxData_.surfaces[i].vertexIndex == PMXModelData::NO_DATA_FLAG){
			return false;
		}
	}

	// テクスチャ -----------------------------
	int numOfTexture = 0;
	tmpStream.read(reinterpret_cast<char*>(&numOfTexture), 4);

	texPtrSize_ = numOfTexture;

	pmxData_.texturePaths.resize(numOfTexture);

	// テクスチャ読み込み
	wstring texturePath{};
	for(int i = 0; i < numOfTexture; ++i){
		GetPMXStringUTF16S(tmpStream, texturePath);

		string t = WStringToString(texturePath);

		pmxData_.texturePaths[i] = t;

		Texture* pTex = new Texture;
		pTex->LoadImageFromArchiveFile(pArchiveFileName, t.c_str());
		vecTexPtr_.push_back(pTex);
	}

	// マテリアル -----------------------------
	int numOfMaterial = 0;
	tmpStream.read(reinterpret_cast<char*>(&numOfMaterial), 4);

	materialSize_ = numOfMaterial;

	pmxData_.materials.resize(numOfMaterial);

	for(int i = 0; i < numOfMaterial; ++i){
		for(int j = 0; j < 2; ++j){
			tmpStream.read(reinterpret_cast<char*>(&arrayLength), 4);
			for(unsigned int k = 0; k < arrayLength; ++k){ tmpStream.get(); }
		}

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].diffuse), 16);
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].specular), 12);
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].specularity), 4);
		tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].ambient), 12);

		tmpStream.get();

		for(int j = 0; j < 16; ++j){ tmpStream.get(); }

		for(int j = 0; j < 4; ++j){ tmpStream.get(); }

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].colorMapTextureIndex), hederData[TEXTURE_INDEX_SIZE]);

		for(unsigned char j = 0; j < hederData[TEXTURE_INDEX_SIZE]; ++j){ tmpStream.get(); }

		tmpStream.get();

		const byte shareToonFlag = tmpStream.get();

		if(shareToonFlag){
			tmpStream.get();
		} else{
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].toonTextureIndex), hederData[TEXTURE_INDEX_SIZE]);
		}

		tmpStream.read(reinterpret_cast<char*>(&arrayLength), 4);

		for(unsigned int j = 0; j < arrayLength; ++j){ tmpStream.get(); }

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.materials[i].vertexNum), 4);
	}

	// ボーン ---------------------------------
	enum BoneFlagMask{
		ACCESS_POINT = 0x0001,
		IK = 0x0020,
		IMPART_TRANSLATION = 0x0100,
		IMPART_ROTATION = 0x0200,
		AXIS_FIXING = 0x0400,
		LOCAL_AXIS = 0x0800,
		EXTERNAL_PARENT_TRANS = 0x2000,
	};

	int numOfBone = 0;
	tmpStream.read(reinterpret_cast<char*>(&numOfBone), 4);

	boneSize_ = numOfBone;

	pmxData_.bones.resize(numOfBone);

	int ikLinkSize = 0;
	unsigned char angleLim = 0;

	for(int i = 0; i < numOfBone; ++i){
		wstring buf;
		GetPMXStringUTF16S(tmpStream, buf);
		pmxData_.bones[i].name = WStringToString(buf);

		tmpStream.read(reinterpret_cast<char*>(&arrayLength), 4);
		pmxData_.bones[i].nameEnglish.resize(arrayLength);

		for(unsigned int j = 0; j < arrayLength; ++j){
			pmxData_.bones[i].nameEnglish[j] = tmpStream.get();
		}

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].position), 12);

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].parentIndex), hederData[BONE_INDEX_SIZE]);

		if(numOfBone <= pmxData_.bones[i].parentIndex){
			pmxData_.bones[i].parentIndex = PMXModelData::NO_DATA_FLAG;
		}

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].transformationLevel), 4);

		tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].flag), 2);

		if(pmxData_.bones[i].flag & ACCESS_POINT){
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].childrenIndex), hederData[BONE_INDEX_SIZE]);
			if(numOfBone <= pmxData_.bones[i].childrenIndex)
			{
				pmxData_.bones[i].childrenIndex = PMXModelData::NO_DATA_FLAG;
			}
		} else{
			pmxData_.bones[i].childrenIndex = PMXModelData::NO_DATA_FLAG;
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].coordOffset), 12);
		}

		if((pmxData_.bones[i].flag & IMPART_TRANSLATION) || (pmxData_.bones[i].flag & IMPART_ROTATION)){
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].impartParentIndex), hederData[BONE_INDEX_SIZE]);
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].impartRate), 4);
		}

		if(pmxData_.bones[i].flag & AXIS_FIXING){
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].fixedAxis), 12);
		}

		if(pmxData_.bones[i].flag & LOCAL_AXIS){
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].localAxisX), 12);
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].localAxisZ), 12);
		}

		if(pmxData_.bones[i].flag & EXTERNAL_PARENT_TRANS){
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].externalParentKey), 4);
		}

		if(pmxData_.bones[i].flag & IK){
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikTargetIndex), hederData[5]);
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLoopCount), 4);
			tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikUnitAngle), 4);
			tmpStream.read(reinterpret_cast<char*>(&ikLinkSize), 4);

			pmxData_.bones[i].ikLinks.resize(ikLinkSize);

			for(int j = 0; j < ikLinkSize; ++j){
				tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].index), hederData[5]);
				angleLim = tmpStream.get();

				pmxData_.bones[i].ikLinks[j].existAngleLimited = false;

				if(angleLim == 1){
					tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].limitAngleMin), 12);
					tmpStream.read(reinterpret_cast<char*>(&pmxData_.bones[i].ikLinks[j].limitAngleMax), 12);
					pmxData_.bones[i].ikLinks[j].existAngleLimited = true;
				}
			}
		} else{
			pmxData_.bones[i].ikTargetIndex = PMXModelData::NO_DATA_FLAG;
		}
	}
	for(unsigned int i = 0; i < boneSize_; ++i){
		for(auto& ikl : pmxData_.bones[i].ikLinks){
			if(ikl.index != PMXModelData::NO_DATA_FLAG){
				ikl.name = pmxData_.bones[ikl.index].name;
			}
		}
	}

	vecVertex_.resize(vertexSize_);

	for(unsigned int i = 0; i < vertexSize_; ++i){
		vecVertex_[i].pos.x = pmxData_.vertices[i].position.x;
		vecVertex_[i].pos.y = pmxData_.vertices[i].position.y;
		vecVertex_[i].pos.z = pmxData_.vertices[i].position.z;

		vecVertex_[i].nor.x = pmxData_.vertices[i].normal.x;
		vecVertex_[i].nor.y = pmxData_.vertices[i].normal.y;
		vecVertex_[i].nor.z = pmxData_.vertices[i].normal.z;

		vecVertex_[i].tex.x = pmxData_.vertices[i].uv.x;
		vecVertex_[i].tex.y = pmxData_.vertices[i].uv.y;

		vecVertex_[i].col = ideaColor::WHITE;

		vecVertex_[i].boneIndex[0] = pmxData_.vertices[i].weight.born1;
		vecVertex_[i].boneIndex[1] = pmxData_.vertices[i].weight.born2;
		vecVertex_[i].boneIndex[2] = pmxData_.vertices[i].weight.born3;
		vecVertex_[i].boneIndex[3] = pmxData_.vertices[i].weight.born4;

		vecVertex_[i].weight[0] = pmxData_.vertices[i].weight.weight1;
		vecVertex_[i].weight[1] = pmxData_.vertices[i].weight.weight2;
		vecVertex_[i].weight[2] = pmxData_.vertices[i].weight.weight3;
	}

	vecIndex_.resize(indexSize_);

	for(unsigned int i = 0; i < indexSize_; ++i){
		vecIndex_[i] = pmxData_.surfaces[i].vertexIndex;
	}

	vecMaterial_.resize(materialSize_);

	for(unsigned int i = 0; i < materialSize_; ++i){
		vecMaterial_[i].diffuse = pmxData_.materials[i].diffuse;
		vecMaterial_[i].specular = pmxData_.materials[i].specular;
		vecMaterial_[i].specularity = pmxData_.materials[i].specularity;
		vecMaterial_[i].ambient = pmxData_.materials[i].ambient;
		vecMaterial_[i].colorMapTextureIndex = pmxData_.materials[i].colorMapTextureIndex;
		vecMaterial_[i].vertexNum = pmxData_.materials[i].vertexNum;
	}

	// ボーン情報格納
	vecBone_.resize(boneSize_);
	vector<string> vecBoneName(boneSize_);

	for(unsigned int i = 0; i < boneSize_; ++i){
		auto& pd = pmxData_.bones[i];

		vecBoneName[i] = pd.name;
		auto& node = mapBone_[pd.name];
		node.index = i;
		node.name = pd.name;

		if(node.name.find("ひざ") != std::string::npos) {
			vecKneeIndex.emplace_back(i);
		}

		node.parentIndex = pd.parentIndex;
		if(pd.parentIndex != -1){
			node.parentName = pmxData_.bones[pd.parentIndex].name;
		} else{
			node.parentName = "";
		}

		if(pd.flag & IK){
			node.type = 3;

			node.ikTargetIndex = pd.ikTargetIndex;
			if(pd.ikTargetIndex != -1){
				node.ikTargetName = pmxData_.bones[pd.ikTargetIndex].name;
			} else{
				node.ikTargetName = "";
			}
			node.ikLoopCount = pd.ikLoopCount;
			node.ikUnitAngle = pd.ikUnitAngle;
			copy(pd.ikLinks.begin(), pd.ikLinks.end(), back_inserter(node.ikLinks));
			node.ikLinkSize = (int)pd.ikLinks.size();

			VmdIkData ik;

			ik.boneIdx = node.index;
			ik.name = node.name;
			ik.targetIdx = node.ikTargetIndex;
			ik.targetName = node.ikTargetName;
			ik.loopCount = node.ikLoopCount;
			ik.limit = node.ikUnitAngle;
			for(int i = 0; i < node.ikLinkSize; ++i){
				ik.nodeIdxes.push_back(node.ikLinks[i].index);
				ik.nodeNames.push_back(node.ikLinks[i].name);
			}
			ik.nodeSize = ik.nodeIdxes.size();

			vecIk_.push_back(ik);
		} else{
			node.type = 0;

			node.ikTargetIndex = -1;
			node.ikTargetName = "";
			node.ikLoopCount = 0;
			node.ikUnitAngle = 0.0f;
			node.ikLinkSize = 0;
		}

		node.pos = pd.position;

		node.ikParentBone = -1;
		node.ikParentName = "";

		vecBone_[i] = node;
	}

	kneeIndexSize_ = (unsigned long)vecKneeIndex.size();

	ikSize_ = (unsigned short)vecIk_.size();

	for(int i = 0; i < (int)ikSize_; ++i){
		for(int j = 0; j < (int)vecIk_[i].nodeSize; ++j){
			vecBone_[vecIk_[i].nodeIdxes[j]].ikParentBone = vecIk_[i].boneIdx;
			vecBone_[vecIk_[i].nodeIdxes[j]].ikParentName = vecIk_[i].name;
		}
	}

	for(unsigned int i = 0; i < boneSize_; ++i){
		mapBone_[vecBoneName[i]] = vecBone_[i];
	}

	for(auto& pd : pmxData_.bones){
		if(pd.parentIndex >= boneSize_ || pd.parentIndex == PMXModelData::NO_DATA_FLAG){
			continue;
		}
		auto parentName = vecBoneName[pd.parentIndex];
		mapBone_[parentName].vecChildrenPtr.emplace_back(&mapBone_[pd.name]);
		vecBone_[mapBone_[parentName].index].vecChildrenPtr.emplace_back(&mapBone_[pd.name]);
	}

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4Identity());

	{
		// 頂点バッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(BlendVertexData) * vertexSize_;
		bd.Usage = D3D11_USAGE_DYNAMIC;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecVertex_[0];
		if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pVertexBuffer_))){ return false; }
	}

	{
		// インデックスバッファを作成
		D3D11_BUFFER_DESC bd;
		bd.ByteWidth = sizeof(unsigned short) * indexSize_;
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		InitData.pSysMem = &vecIndex_[0];
		InitData.SysMemPitch = 0;
		InitData.SysMemSlicePitch = 0;
		if(FAILED(gm.GetDevicePtr()->CreateBuffer(&bd, &InitData, &pIndexBuffer_))){ return false; }

	}

	DebugManager::Instance().AddResource(pFileName);

	return true;
}

bool PmxModel::LoadPmxMeshFromStorage(const char* pFileName)
{
	if(pVertexBuffer_ || pIndexBuffer_){ return false; }	// 既に読み込み済みなら終了

	PmxModel& pmx = StorageManager::Instance().GetPmxModel(pFileName);

	// ストレージから読み込む
	// 頂点
	vertexSize_ = pmx.vertexSize_;
	pVertexBuffer_ = pmx.pVertexBuffer_;
	copy(pmx.vecVertex_.begin(), pmx.vecVertex_.end(), back_inserter(vecVertex_));

	// インデックス
	indexSize_ = pmx.indexSize_;
	pIndexBuffer_ = pmx.pIndexBuffer_;
	copy(pmx.vecIndex_.begin(), pmx.vecIndex_.end(), back_inserter(vecIndex_));

	// マテリアル -----------------------------
	materialSize_ = pmx.materialSize_;
	copy(pmx.vecMaterial_.begin(), pmx.vecMaterial_.end(), back_inserter(vecMaterial_));

	// テクスチャ -----------------------------
	texPtrSize_ = pmx.texPtrSize_;
	copy(pmx.vecTexPtr_.begin(), pmx.vecTexPtr_.end(), back_inserter(vecTexPtr_));

	// ボーン -----------------------------
	boneSize_ = pmx.boneSize_;
	copy(pmx.vecBone_.begin(), pmx.vecBone_.end(), back_inserter(vecBone_));
	mapBone_ = pmx.mapBone_;

	ikSize_ = pmx.ikSize_;
	copy(pmx.vecIk_.begin(), pmx.vecIk_.end(), back_inserter(vecIk_));

	kneeIndexSize_ = pmx.kneeIndexSize_;
	copy(pmx.vecKneeIndex.begin(), pmx.vecKneeIndex.end(), back_inserter(vecKneeIndex));

	vecBoneMatrix_.resize(boneSize_);

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4Identity());

	// ストレージ使用フラグをオンに
	if(pVertexBuffer_ && pIndexBuffer_){
		bStorage_ = true;
	}

	return true;
}

void PmxModel::UnLoad()
{
	if(!bStorage_){
		SafeRelease(pIndexBuffer_);

		SafeRelease(pVertexBuffer_);

		for(auto it = vecTexPtr_.begin(); it != vecTexPtr_.end(); ++it){
			if(*it){
				(*it)->UnLoad();
			}
			SafeDelete((*it));
		}
	}

	bStorage_ = false;

	pVertexBuffer_ = nullptr;
	pIndexBuffer_ = nullptr;

	vertexSize_ = 0;
	vector<BlendVertexData>().swap(vecVertex_);

	indexSize_ = 0;
	vector<unsigned short>().swap(vecIndex_);

	materialSize_ = 0;
	vector< PMXModelData::Material>().swap(vecMaterial_);

	texPtrSize_ = 0;
	vector<Texture*>().swap(vecTexPtr_);

	boneSize_ = 0;
	vector<VmdBone>().swap(vecBone_);
	unordered_map<string, VmdBone>().swap(mapBone_);
	vector<Matrix4x4>().swap(vecBoneMatrix_);

	ikSize_ = 0;
	vector<VmdIkData>().swap(vecIk_);

	kneeIndexSize_ = 0;
	vector<unsigned long>().swap(vecKneeIndex);
}

void PmxModel::UpdateVmd(VmdMotion& vmd, float speed, float blendSpeed, bool loop, bool bOfset)
{
	if(!pVertexBuffer_){ return; }

	if(!pPreMotion_){
		pPreMotion_ = &vmd;
		bFinish_ = false;
	}

	elapsedTime_ += 0.5f * speed;

	unsigned long frameNo = static_cast<unsigned long>(elapsedTime_);

	if(frameNo >= vmd.GetFrameMax()){
		if(loop){
			elapsedTime_ = 0.0f;
			frameNo = 0;
		} else{
			bFinish_ = true;
		}
	}

	if(pPreMotion_ != &vmd){
		if(!bBlend_){
			preLastFranme_ = frameNo;
			bFinish_ = false;
		}

		bBlend_ = true;
	} else{
		bBlend_ = false;
	}

	if(bBlend_){
		blendRate_ += blendSpeed;

		elapsedTime_ = 0.0f;
		frameNo = 0U;
	} else{
		blendRate_ = 0.0f;
	}

	if(blendRate_ >= 1.0f){
		pPreMotion_ = &vmd;

		bBlend_ = false;
	}

	fill(vecBoneMatrix_.begin(), vecBoneMatrix_.end(), Matrix4x4Identity());

	if(bBlend_ && pPreMotion_){
		for(auto& md : vmd.umapMotionData_){
			auto itBone = mapBone_.find(md.first);
			if(itBone == mapBone_.end()){ continue; }

			auto node = itBone->second;
			if(node.index == -1){ continue; }

			auto motions = md.second;

			std::vector<VmdMotion::KeyFrame> motions_p;

			if(pPreMotion_->umapMotionData_.find(md.first) != pPreMotion_->umapMotionData_.end()){
				motions_p = pPreMotion_->umapMotionData_[md.first];
				frameNo = preLastFranme_;
			} else{
				motions_p = md.second;
				frameNo = 0U;
			}

			auto rit = find_if(
				motions.rbegin(), motions.rend(),
				[](const VmdMotion::KeyFrame& motion)
				{
					return motion.frameNo == 0;
				}
			);

			if(rit == motions.rend()){ continue; }

			XMMATRIX rotation = XMMatrixIdentity();
			auto xmf1 = Vector3DtoXMFLOAT3(rit->offset);

			XMVECTOR offset = XMLoadFloat3(&xmf1);

			auto rit_p = find_if(
				motions_p.rbegin(), motions_p.rend(),
				[frameNo](const VmdMotion::KeyFrame& motion)
				{
					return motion.frameNo <= frameNo;
				}
			);

			if(rit_p == motions_p.rend()){ continue; }

			XMVECTOR  pq;

			auto xmf1_p = Vector3DtoXMFLOAT3(rit_p->offset);
			XMVECTOR offset_p = XMLoadFloat3(&xmf1_p);

			auto it_p = rit_p.base();

			if(it_p != motions_p.end()){
				XMVECTOR pq1 = XMVectorSet(rit_p->quaternion.x, rit_p->quaternion.y, rit_p->quaternion.z, rit_p->quaternion.w);
				XMVECTOR pq2 = XMVectorSet(it_p->quaternion.x, it_p->quaternion.y, it_p->quaternion.z, it_p->quaternion.w);

				pq = XMQuaternionSlerp(pq1, pq2, t_);

				auto xmf2 = Vector3DtoXMFLOAT3(it_p->offset);

				offset_p = XMVectorLerp(offset_p, XMLoadFloat3(&xmf2), t_);
			} else{
				pq = XMVectorSet(rit_p->quaternion.x, rit_p->quaternion.y, rit_p->quaternion.z, rit_p->quaternion.w);
			}

			XMVECTOR q1 = XMVectorSet(rit->quaternion.x, rit->quaternion.y, rit->quaternion.z, rit->quaternion.w);

			XMVECTOR  q = XMQuaternionSlerp(pq, q1, blendRate_);

			rotation = XMMatrixRotationQuaternion(q);

			offset = XMVectorLerp(offset_p, offset, blendRate_);

			auto& pos = node.pos;

			if(bOfset){
				auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
					* rotation
					* XMMatrixTranslationFromVector(offset)
					* XMMatrixTranslation(pos.x, pos.y, pos.z);
				vecBoneMatrix_[node.index] = XMMATRIXtoMatrix4x4(mat);
			} else{
				auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
					* rotation
					* XMMatrixTranslation(pos.x, pos.y, pos.z);
				vecBoneMatrix_[node.index] = XMMATRIXtoMatrix4x4(mat);
			}
		}
	} else{
		for(auto& md : vmd.umapMotionData_){
			auto itBone = mapBone_.find(md.first);
			if(itBone == mapBone_.end()){ continue; }

			auto node = itBone->second;
			if(node.index == -1){ continue; }

			auto motions = md.second;

			auto rit = find_if(
				motions.rbegin(), motions.rend(),
				[frameNo](const VmdMotion::KeyFrame& motion)
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

				t_ = t = Bezier(t, it->p1, it->p2, 12);

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
				vecBoneMatrix_[node.index] = XMMATRIXtoMatrix4x4(mat);
			} else{
				auto mat = XMMatrixTranslation(-pos.x, -pos.y, -pos.z)
					* rotation
					* XMMatrixTranslation(pos.x, pos.y, pos.z);
				vecBoneMatrix_[node.index] = XMMATRIXtoMatrix4x4(mat);
			}
		}
	}

	RecursiveMatrixMultiply(&mapBone_["センター"], Matrix4x4Identity());
}

void PmxModel::Draw(Camera* pCamera)
{
	GraphicManager& gm = GraphicManager::Instance();
	ObjectManager& om = ObjectManager::Instance();

	// 準備ができていなければ終了
	if(!pVertexBuffer_
		|| !pIndexBuffer_){
		return;
	}

	//定数バッファ
	{
		ConstBuffer3D cbuff = {};

		XMFLOAT4X4 matWorld = Matrix4x4toXMFLOAT4X4(world_);
		XMStoreFloat4x4(&cbuff.world, XMMatrixTranspose(XMLoadFloat4x4(&matWorld)));

		XMFLOAT4X4 matView = Matrix4x4toXMFLOAT4X4(pCamera->GetViewMatrix());
		XMStoreFloat4x4(&cbuff.view, XMMatrixTranspose(XMLoadFloat4x4(&matView)));

		XMFLOAT4X4 matProj = Matrix4x4toXMFLOAT4X4(pCamera->GetProjectionMatrix());
		XMStoreFloat4x4(&cbuff.proj, XMMatrixTranspose(XMLoadFloat4x4(&matProj)));

		XMStoreFloat4(&cbuff.color, XMVectorSet(color_.r, color_.g, color_.b, color_.a));

		XMVECTOR light = {};

		light.m128_f32[0] = om.GetLight().x;
		light.m128_f32[1] = om.GetLight().y;
		light.m128_f32[2] = om.GetLight().z;

		XMStoreFloat4(&cbuff.light, light);

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(om.GetConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファをセット
		UINT cb_slot = 1;
		ID3D11Buffer* cb[] = { om.GetConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	//定数バッファ(ボーンマトリクス)
	{
		BoneWorldConstBuffer cbuff = {};

		vector<XMMATRIX> worlds;
		worlds.resize(boneSize_);

		// ワールド行列の初期化
		XMMATRIX matIdentity = XMMatrixIdentity();

		for(unsigned int i = 0; i < 512; ++i){
			XMStoreFloat4x4(&cbuff.boneWorld[i], matIdentity);
		}

		for(unsigned int i = 0; i < boneSize_ && i < 512; ++i){
			XMStoreFloat4x4(&cbuff.boneWorld[i], Matrix4x4toXMMATRIX(vecBoneMatrix_[i]));
		}

		// 定数バッファ内容更新
		gm.GetContextPtr()->UpdateSubresource(om.GetWeightConstBufferPtr(), 0, NULL, &cbuff, 0, 0);

		// 定数バッファ
		UINT cb_slot = 2;
		ID3D11Buffer* cb[] = { om.GetWeightConstBufferPtr() };
		gm.GetContextPtr()->VSSetConstantBuffers(cb_slot, 1, cb);
	}

	//バーテックスバッファをセット
	ID3D11Buffer* pVBuf = pVertexBuffer_;
	UINT stride = sizeof(BlendVertexData);
	UINT offset = 0;
	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	//インデックスバッファをセット
	gm.GetContextPtr()->IASetIndexBuffer(pIndexBuffer_, DXGI_FORMAT_R16_UINT, 0);

	// 入力レイアウトのセット
	gm.GetContextPtr()->IASetInputLayout(om.GetWeightInputLayoutPtr());

	// プリミティブ形状のセット
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// ラスタライザステート
	gm.GetContextPtr()->RSSetState(gm.GetDefaultRasterizerStatePtr());

	// デプスステンシルステート
	gm.GetContextPtr()->OMSetDepthStencilState(gm.GetDefaultDepthStatePtr(), 0);

	// シェーダのセット
	gm.GetContextPtr()->VSSetShader(om.GetWeightVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// マテリアル毎に描画
	if(materialSize_ <= 1){
		ID3D11ShaderResourceView* pTexView = nullptr;

		if(texPtrSize_ > 0 && vecMaterial_[0].colorMapTextureIndex != 255){
			pTexView = vecTexPtr_[vecMaterial_[0].colorMapTextureIndex]->GetTextureViewPtr();
		}

		if(pTexView){
			gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
		} else{
			gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
		}

		gm.GetContextPtr()->DrawIndexed(indexSize_, 0, 0);
	} else{
		unsigned long strat = 0;

		for(unsigned int i = 0; i < materialSize_; ++i){
			ID3D11ShaderResourceView* pTexView = nullptr;

			if(texPtrSize_ > 0 && vecMaterial_[i].colorMapTextureIndex != 255){
				pTexView = vecTexPtr_[vecMaterial_[i].colorMapTextureIndex]->GetTextureViewPtr();
			}

			if(pTexView){
				gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederTexturePtr(), NULL, 0);
			} else{
				gm.GetContextPtr()->PSSetShader(om.GetPixelShederDefaultPtr(), NULL, 0);
			}

			gm.GetContextPtr()->DrawIndexed(vecMaterial_[i].vertexNum, strat, 0);

			strat += vecMaterial_[i].vertexNum;
		}
	}
}

void PmxModel::RecursiveMatrixMultiply(VmdBone* node, const idea::Matrix4x4& mat)
{
	if(node->index == -1){ return; }

	vecBoneMatrix_[node->index] *= mat;

	for(auto& cnode : node->vecChildrenPtr){
		RecursiveMatrixMultiply(cnode, vecBoneMatrix_[node->index]);
	}
}
