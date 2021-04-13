#ifndef INCLUDE_IDEA_OBJECTMANAGER_H
#define INCLUDE_IDEA_OBJECTMANAGER_H

#include "../Shader/Shader.h"
#include "../Utility/ideaMath.h"

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11ShaderResourceView;
struct ID3D11InputLayout;

namespace idea{
	class ObjectManager{
	public:
		static const int CUBE_VERTEX_NUM = 24;
		static const int CUBE_INDEX_NUM = 36;

		static ObjectManager& Instance()	// 唯一のインスタンスを返す
		{
			static ObjectManager s_Instance;
			return s_Instance;
		}

		bool Init();
		void UnInit();

		ID3D11VertexShader* GetVertexShederPtr()const{ return pVertexShader_; }
		ID3D11VertexShader* GetBillboardVertexShederPtr()const{ return pBillboardVertexShader_; }
		ID3D11VertexShader* GetShadowVertexShederPtr()const{ return pShadowVertexShader_; }
		ID3D11VertexShader* GetWeightVertexShederPtr()const{ return pWeightVertexShader_; }

		ID3D11InputLayout* GetInputLayoutPtr()const{ return pVertexLayout_; }
		ID3D11InputLayout* GetBillboardInputLayoutPtr()const{ return pBillboardVertexLayout_; }
		ID3D11InputLayout* GetShadowInputLayoutPtr()const{ return pShadowVertexLayout_; }
		ID3D11InputLayout* GetWeightInputLayoutPtr()const{ return pWeightVertexLayout_; }

		ID3D11PixelShader* GetPixelShederDefaultPtr()const{ return pixelShaderDefault_.GetPixelShaderPtr(); }
		ID3D11PixelShader* GetPixelShaderShadowPtr()const{ return pixelShaderShadow_.GetPixelShaderPtr(); }
		ID3D11PixelShader* GetPixelShederTexturePtr()const{ return pixelShaderTexture_.GetPixelShaderPtr(); }
		ID3D11PixelShader* GetPixelShaderTextureShadowPtr()const{ return pixelShaderTextureShadow_.GetPixelShaderPtr(); }

		ID3D11Buffer* GetConstBufferPtr()const{ return pConstBuffer_; }
		ID3D11Buffer* GetWorldConstBufferPtr()const{ return pWorldConstBuffer_; }
		ID3D11Buffer* GetVPConstBufferPtr()const{ return pVPConstBuffer_; }
		ID3D11Buffer* GetLightConstBufferPtr()const{ return pLightConstBuffer_; }
		ID3D11Buffer* GetCameraConstBufferPtr()const{ return pCameraConstBuffer_; }
		ID3D11Buffer* GetShadowConstBufferPtr()const{ return pShadowConstBuffer_; }
		ID3D11Buffer* GetMaterialConstBufferPtr()const{ return pMaterialConstBuffer_; }
		ID3D11Buffer* GetWeightConstBufferPtr()const{ return pWeightConstBuffer_; }

		ID3D11Buffer* GetCubeVertexBufferPtr()const{ return pCubeVertexBuffer_; }
		ID3D11Buffer* GetCubeIndexBufferPtr()const{ return pCubeIndexBuffer_; }
		UINT GetCubeIndexSize()const{ return cubeIndexSize_; }

		ID3D11Buffer* GetSphereVertexBufferPtr()const{ return pSphereVertexBuffer_; }
		ID3D11Buffer* GetSphereIndexBufferPtr()const{ return pSphereIndexBuffer_; }
		UINT GetSphereIndexSize()const{ return sphereIndexSize_; }

		ID3D11Buffer* GetBillboardVertexBufferPtr()const{ return pBillboardVertexBuffer_; }
		ID3D11Buffer* GetBillboardIndexBufferPtr()const{ return pBillboardIndexBuffer_; }

		static void SetLight(float axisX, float axisY, float axisZ);
		static const Vector3D& GetLight(){ return s_Light_; }

	public:
		ID3D11VertexShader* pVertexShader_ = nullptr;
		ID3D11VertexShader* pBillboardVertexShader_ = nullptr;
		ID3D11VertexShader* pShadowVertexShader_ = nullptr;
		ID3D11VertexShader* pWeightVertexShader_ = nullptr;

		PixelShader pixelShaderDefault_;
		PixelShader pixelShaderShadow_;
		PixelShader pixelShaderTexture_;
		PixelShader pixelShaderTextureShadow_;

		ID3D11InputLayout* pVertexLayout_ = nullptr;
		ID3D11InputLayout* pBillboardVertexLayout_ = nullptr;
		ID3D11InputLayout* pShadowVertexLayout_ = nullptr;
		ID3D11InputLayout* pWeightVertexLayout_ = nullptr;

		ID3D11Buffer* pConstBuffer_ = nullptr;///
		ID3D11Buffer* pWorldConstBuffer_ = nullptr;
		ID3D11Buffer* pVPConstBuffer_ = nullptr;
		ID3D11Buffer* pLightConstBuffer_ = nullptr;
		ID3D11Buffer* pCameraConstBuffer_ = nullptr;
		ID3D11Buffer* pMaterialConstBuffer_ = nullptr;
		ID3D11Buffer* pWeightConstBuffer_ = nullptr;
		ID3D11Buffer* pShadowConstBuffer_ = nullptr;///

		ID3D11Buffer* pCubeVertexBuffer_ = nullptr;
		ID3D11Buffer* pCubeIndexBuffer_ = nullptr;
		UINT cubeIndexSize_ = 0U;

		ID3D11Buffer* pSphereVertexBuffer_ = nullptr;
		ID3D11Buffer* pSphereIndexBuffer_ = nullptr;
		UINT sphereIndexSize_ = 0U;

		ID3D11Buffer* pBillboardVertexBuffer_ = nullptr;
		ID3D11Buffer* pBillboardIndexBuffer_ = nullptr;

		static Vector3D s_Light_;

		ObjectManager();				// コンストラクタ
		~ObjectManager(){ UnInit(); }	// デストラクタ

		// コピーコンストラクタの禁止
		ObjectManager(const ObjectManager& src){}
		ObjectManager& operator=(const ObjectManager& src){}
	};
}
#endif	// #ifndef INCLUDE_IDEA_OBJECTMANAGER_H