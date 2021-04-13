#ifndef INCLUDE_EIDOS_PLAINMESH_H
#define INCLUDE_EIDOS_PLAINMESH_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include "../3D/Object.h"
#include "../3D/3DStruct.h"
#include "../Texture/TextureHolder.h"
#include <vector>

//------------------------------------------------------------------------------
// �O���錾
//------------------------------------------------------------------------------
struct ID3D11Buffer;

namespace idea{
	class Camera;
	class ShadowCamera;

	class PlaneMesh : public Object, public TextureHolder{
	public:
		PlaneMesh();
		~PlaneMesh(){ Release(); }

		bool Create(float centerX, float centerZ, float widthX, float widthZ, unsigned int uNum = 1U, unsigned int vNum = 1U, bool bOnePicture = false);

		void Release();

		void SetTexture(Texture& tex);
		void ExclusionTexture();

		void SetShadow(ShadowCamera& scmr);

		float GetHeight(float posX, float posZ);
		float GetHeight(const Vector3D& position);

		Vector3D GetNormal(float posX, float posZ);
		Vector3D GetNormal(const Vector3D& position);

	private:
		float widthX_ = 0.0f;
		float widthZ_ = 0.0f;

		unsigned int uNum_ = 0U;
		unsigned int vNum_ = 0U;

		std::vector<Vector3D> vertexList_;

		int vertexSize_ = 0;

		ID3D11Buffer* pVertexBuffer_ = nullptr;

		ShadowCamera* pScmr_ = nullptr;

		void Draw(Camera* pCamera)override;
		inline void DrawPlain(Camera* pCamera, int blend = 0);	// ��`�̕`��
		inline void DrawPlainShadow(Camera* pCamera, int blend = 0);	// ��`�̕`��
		inline void DrawTexturePlain(Camera* pCamera, const Texture& tex, int blend = 0);	// ��`�̕`��
		inline void DrawTexturePlainShadow(Camera* pCamera, const Texture& tex, int blend = 0);	// ��`�̕`��
	};
}

#endif	// #ifndef INCLUDE_EIDOS_PLAINMESH_H