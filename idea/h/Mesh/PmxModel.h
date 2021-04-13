#ifndef INCLUDE_EIDOS_PMXMODEL_H
#define INCLUDE_EIDOS_PMXMODEL_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
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

		// �t�@�C�����烁�b�V����ǂݍ���
		bool LoadPmxMeshFromFile(const char* pFileName);

		// �A�[�J�C�u�t�@�C�����烁�b�V����ǂݍ���
		bool LoadPmxMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// �X�g���[�W���烁�b�V����ǂݍ���
		bool LoadPmxMeshFromStorage(const char* pFileName);

		// ���b�V���̔j��
		void UnLoad();

		void UpdateVmd(idea::VmdMotion& vmd, float speed = 1.0f, float blendSpeed = 0.1f, bool loop = true, bool bOfset = true);

		bool IsFinishMotion()const
		{
			return bFinish_;
		}

	private:
		friend class idea::VmdMotion;

		//���_�o�b�t�@
		ID3D11Buffer* pVertexBuffer_ = nullptr;
		
		//�C���f�b�N�X�o�b�t�@
		ID3D11Buffer* pIndexBuffer_ = nullptr;

		// ���_�T�C�Y
		unsigned long vertexSize_ = 0U;

		// ���_���
		std::vector<BlendVertexData> vecVertex_;

		// �C���f�b�N�X�T�C�Y
		unsigned long indexSize_ = 0U;

		// �C���f�b�N�X���
		std::vector<unsigned short> vecIndex_;

		// �}�e���A���T�C�Y
		unsigned long materialSize_ = 0U;

		// �}�e���A�����
		std::vector<idea::PMXModelData::Material> vecMaterial_;

		// �e�N�X�`���|�C���^�T�C�Y
		unsigned long texPtrSize_ = 0U;

		// �e�N�X�`���|�C���^
		std::vector<idea::Texture*> vecTexPtr_;

		// �{�[���T�C�Y
		unsigned short boneSize_ = 0U;

		// �{�[�����
		std::vector<VmdBone> vecBone_;
		std::unordered_map<std::string, VmdBone> mapBone_;

		// �{�[���}�g���N�X
		std::vector<idea::Matrix4x4> vecBoneMatrix_;

		// IK���T�C�Y
		unsigned short ikSize_ = 0U;

		// IK���
		std::vector<idea::VmdIkData> vecIk_;

		// �Ђ��C���f�b�N�X�T�C�Y
		unsigned long kneeIndexSize_ = 0U;

		// �Ђ��C���f�b�N�X
		std::vector<unsigned long> vecKneeIndex;

		// �X�g���[�W�g�p�t���O
		bool bStorage_ = false;

		float elapsedTime_ = 0.0f;

		bool bBlend_ = false;

		bool bFinish_ = false;

		float blendRate_ = 0.0f;

		idea::VmdMotion* pPreMotion_ = nullptr;

		unsigned long preLastFranme_ = 0U;

		float t_ = 0.0f;

		//�`��
		void Draw(idea::Camera* pCamera)override;

		void RecursiveMatrixMultiply(VmdBone* node, const idea::Matrix4x4& mat);

		// �R�s�[�R���X�g���N�^�̋֎~
		PmxModel(const idea::PmxModel& src){}
		PmxModel& operator=(const idea::PmxModel& src){}
	};
}
#endif	// #ifndef INCLUDE_EIDOS_PMXMODEL_H
