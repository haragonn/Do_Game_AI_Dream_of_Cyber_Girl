#ifndef INCLUDE_IDEA_3DSTRUCT_H
#define INCLUDE_IDEA_3DSTRUCT_H

#include "../Texture/Texture.h"
#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"

#include <vector>
#include <unordered_map>
#include <string>

namespace idea{
	// ���b�V���̒��_�f�[�^
	struct MeshVertexData{
		Vector3D pos;
		Vector3D nor;
		Color color;
		Vector2D tex;
	};

	// ���b�V���̒��_�f�[�^
	struct BillboardVertexData{
		Vector3D pos;
		Vector2D tex;
	};

	// �u�����h�d�ݕt���b�V���̒��_�f�[�^
	struct BlendVertexData{
		Vector3D pos;
		Vector3D nor;
		Color col;
		Vector2D tex;
		float weight[3];
		unsigned int boneIndex[4];
	};

	// �}�e���A���\����
	struct Material{
		Color ambient;
		Color diffuse;
		Color specular;
		Color emission;
		float shininess;
	};

#pragma pack(push,1)
	// .obj
	// �T�u�Z�b�g
	struct ObjSubset{
		unsigned int materialIndex;
		unsigned int faceStart;
		unsigned int faceCount;
	};

	// �}�e���A���\����
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
	// PMX�̃��f���f�[�^�^
	struct PMXModelData{
		static constexpr int NO_DATA_FLAG = -1;

		// ���_���
		struct Vertex{
			Vector3D position;
			Vector3D normal;
			Vector2D uv;
			std::vector<Vector4D> additionalUV;

			// �{�[���E�F�C�g
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

			// �G�b�W�{��
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

		// �{�[�����
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
	// VMD�̃w�b�_�[
	struct VmdHeader{
		// �}�W�b�N�i���o�["Vocaloid Motion Data 0002"
		char vmdHeader[30];
		
		// �J�����̏ꍇ:"�J�����E�Ɩ�"
		char vmdModelName[20];
	};

	// ���[�V����
	struct VmdMotionData{
		// �{�[����
		char boneName[15];

		// �t���[���ԍ�
		unsigned long frameNo;
		
		// �ړ���
		float location[3];

		// ��]�� (���f�����[�J�����W�n
		Quaternion quaternion;
		
		// �⊮
		unsigned char bezier[64];
	};

	// �\��f�[�^
	struct VmdSkin{
		// �\�
		char skinName[15];
		
		// �t���[���ԍ�
		unsigned long flameNo;
		
		// �\��̐ݒ�l(�\��X���C�_�[�̒l)
		float weight;
	};

	// �J����
	struct VmdCamera{
		// �t���[���ԍ�
		unsigned long frameNo;
		
		// ����
		float distance;
		
		// ���W
		Vector3D pos;
		
		// �I�C���[�p
		Vector3D eulerAngle;
		
		// �⊮
		unsigned char interpolation[24];

		// ���E�p
		unsigned long fov;
		
		// �p�[�X�t���OON/OFF
		unsigned char persFlg;
	};

	// ���C�g�Ɩ��f�[�^
	struct VmdLight{
		
		// �t���[���ԍ�
		unsigned long frameNo;
		
		// ���C�g�F
		Vector3D rgb;
		
		// �����x�N�g��(���s����)
		Vector3D vec;
	};

	// �Z���t�e�f�[�^
	struct VmdSelfShadow{
		// �t���[���ԍ�
		unsigned long frameNo;
		
		//�e���[�h(0:�e�Ȃ��A1:���[�h�P�A2:���[�h�Q)
		unsigned char mode;
		
		//����
		float distance;
	};

	// IK�I���I�t�f�[�^
	struct VmdIKEnable {
		// �t���[���ԍ�
		unsigned long frameNo;

		// �L�����
		std::unordered_map<std::string, bool> ikEnableTable;
	};

#pragma pack(pop)

	// �{�[��
	struct VmdBone{
		VmdBone() : type(0), index(-1){}

		// �{�[��ID�i�ʂ��ԍ��j
		long index = -1;

		// �{�[����
		std::string name;

		// �{�[���^�C�v (MMD�̏ꍇ 0:��] 1:��]�ƈړ� 2:IK 3:�s�� 4:IK�e���� 5:��]�e���� 6:IK�ڑ��� 7:��\�� 8:�P�� 9:��]�^�� )
		unsigned char type = 0;

		// ���W
		Vector3D pos;

		// �e�{�[���ԍ�
		long parentIndex = -1;

		// �e�{�[����
		std::string parentName;

		// IK�e�{�[���ԍ�
		long ikParentBone = -1;

		// IK�e�{�[����
		std::string ikParentName;

		// IK�^�[�Q�b�g�̃{�[���ԍ�
		long ikTargetIndex = -1;

		// IK�^�[�Q�b�g�̃{�[����
		std::string ikTargetName;

		// ���s��
		int ikLoopCount = 0;

		// IK�̊p�x
		float ikUnitAngle = 0.0f;

		// IK�ڑ����X�g
		std::vector<IKLink> ikLinks;

		// IK�ڑ����X�g��
		int ikLinkSize = 0;

		// �q�{�[�����X�g
		std::vector<VmdBone*> vecChildrenPtr;
	};

	// IK �f�[�^
	struct VmdIkData{
		// IK�Ώۂ̃{�[���ԍ�
		unsigned int boneIdx;

		// IK�Ώۂ̃{�[����
		std::string name;

		// �^�[�Q�b�g�ɋ߂Â��邽�߂̃{�[���ԍ�
		unsigned int targetIdx;

		// �^�[�Q�b�g�ɋ߂Â��邽�߂̃{�[����
		std::string targetName;
		
		// ���s��
		unsigned int loopCount;

		// ��񓖂���̉�]����
		float limit;
		
		// �Ԃ̃m�[�h�ԍ�
		std::vector<int> nodeIdxes;
		
		// �Ԃ̃m�[�h�ԍ�
		std::vector<std::string> nodeNames;

		// �Ԃ̃m�[�h��
		unsigned int nodeSize;
	};
}

#endif	// #ifndef INCLUDE_IDEA_3DSTRUCT_H