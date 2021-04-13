#ifndef INCLUDE_IDEA_OBJMODEL_H
#define INCLUDE_IDEA_OBJMODEL_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
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

		//obj���b�V�����t�@�C������ǂݍ���
		bool LoadObjMeshFromFile(const char* pFileName);

		//obj���b�V�����A�[�J�C�u�t�@�C������ǂݍ���
		bool LoadObjMeshFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// obj���b�V�����X�g���[�W����ǂݍ���
		bool LoadObjMeshFromStorage(const char* pFileName);

		// ���b�V���̔j��
		void UnLoad();

	private:
		//���_�o�b�t�@
		ID3D11Buffer* pVertexBuffer_ = nullptr;

		//�C���f�b�N�X�o�b�t�@
		ID3D11Buffer* pIndexBuffer_ = nullptr;

		// ���_��񃊃X�g
		std::vector<MeshVertexData> vecVertex_;

		// ���_��
		unsigned int vertexSize_ = 0U;

		// �C���f�b�N�X���X�g
		std::vector<unsigned short> vecIndex_;

		// �C���f�b�N�X��
		unsigned int indexSize_ = 0U;

		// �T�u�Z�b�g���X�g
		std::vector<ObjSubset> vecSubset_;

		// �T�u�Z�b�g��
		unsigned int subsetSize_ = 0U;

		// �}�e���A�����X�g
		std::vector<ObjMaterial> vecMaterial_;

		// �}�e���A����
		unsigned int materialSize_ = 0U;

		// �e�N�X�`���[�|�C���^�[���X�g
		std::vector<idea::Texture*> vecTexPtr_;

		// �e�N�X�`���[��
		unsigned int texPtrSize_ = 0U;

		// �X�g���[�W�g�p�t���O
		bool bStorage_ = false;

		//�}�e���A���t�@�C�����t�@�C������ǂݍ���
		inline bool LoadMaterialFromFile(const char* pFileName);

		//�}�e���A���t�@�C�����A�[�J�C�u�t�@�C������ǂݍ���
		inline bool LoadMaterialFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// �`��
		void Draw(Camera* pCamera)override;

		// �R�s�[�R���X�g���N�^�̋֎~
		ObjModel(const ObjModel& src){}
		ObjModel& operator=(const ObjModel& src){}
	};
}

#endif	// #ifndef INCLUDE_IDEA_OBJMODEL_H
