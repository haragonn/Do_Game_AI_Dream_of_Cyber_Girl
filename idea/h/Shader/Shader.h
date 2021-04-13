/*==============================================================================
	[Shader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_SHARDER_H
#define INCLUDE_IDEA_SHARDER_H

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

struct ID3D11PixelShader;

namespace idea{
	// シェーダの読み込み
	inline int ReadShader(const char* pFileName, BYTE** byteArray);

	//------------------------------------------------------------------------------
	// クラス名　：PixelShader
	// クラス概要：ピクセルシェーダの読み込みと管理を行うクラス
	//------------------------------------------------------------------------------
	class PixelShader{
	public:
		PixelShader();
		~PixelShader();

		// CSOファイルからピクセルシェーダを読み込む
		bool LoadPixelShaderFromCSO(const char* pFileName);

		// アーカイブファイルの中のCSOファイルからピクセルシェーダを読み込む
		bool LoadPixelShaderFromArchiveFile(const char* pArchiveFileName, const char* pFileName);

		// ピクセルシェーダポインタを取得
		ID3D11PixelShader* GetPixelShaderPtr()const
		{
			return pPixelShader_;
		}

	private:
		// ピクセルシェーダポインタ
		ID3D11PixelShader* pPixelShader_ = nullptr;
	};
}

#endif	// #ifndef INCLUDE_IDEA_SHARDER_H
