#include "../../h/3D/BillboardFont.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/Utility/ideaType.h"
#include "../../h/Utility/ConstBufferType.h"
#include "../../h/Utility/ideaUtility.h"

#define WIN32_LEAN_AND_MEAN
#include <d3d11.h>
#include <locale.h>

using namespace std;
using namespace DirectX;
using namespace idea;

namespace{
	//------------------------------------------------------------------------------
	// マルチバイトの区切りをつける
	//------------------------------------------------------------------------------
	int StrSplit(const char* str, std::vector<char*>* dest)
	{
		int i = 0;

		while(*str != '\0') {
			int code = (unsigned char)*str;

			if(code < 0x80) {
				char* arr = new char[2];
				sprintf_s(arr, 2, "%c", *str);
				++i;
				dest->push_back(arr);
				str += 1;
			} else if(code < 0xE0) {
				char* arr = new char[3];
				sprintf_s(arr, 3, "%c%c", *str, *(str + 1));
				++i;
				dest->push_back(arr);
				str += 2;
			} else if(code < 0xF0) {
				char* arr = new char[4];
				sprintf_s(arr, 4, "%c%c%c", *str, *(str + 1), *(str + 2));
				++i;
				dest->push_back(arr);
				str += 3;
			} else {
				char* arr = new char[5];
				sprintf_s(arr, 5, "%c%c%c%c", *str, *(str + 1), *(str + 2), *(str + 3));
				++i;
				dest->push_back(arr);
				str += 4;
			}
		}

		dest->push_back(NULL);

		return i;
	}
}

BillboardFont::BillboardFontTextureData BillboardFont::s_NullTextureData_;
Texture BillboardFont::pNullTexture_;

idea::BillboardFont::BillboardFont() :
	hf_(NULL),
	size_(0),
	bAddFlag_(false){}

idea::BillboardFont::~BillboardFont()
{
	UnInit();
}

void BillboardFont::Init(const Vector3D& position, Camera& camera, int size, int weight, const char* pFontName)
{
	if(hf_ != NULL){ return; }

	position_ = position;

	LOGFONT lf =
	{
		size, 0, 0, 0, weight, 0, 0, 0,
		SHIFTJIS_CHARSET, OUT_TT_ONLY_PRECIS, CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY, DEFAULT_PITCH | FF_MODERN,
		"ＭＳ ゴシック"
	};

	strcpy_s(lf.lfFaceName, pFontName);

	hf_ = CreateFontIndirect(&lf);

	size_ = size;

	billboard_.Init(position_);

	camera.AddObject(billboard_);
}

void BillboardFont::UnInit()
{
	if(hf_){
		DeleteObject(hf_);
		hf_ = NULL;
	}

	for(auto& td : umTextureData_){
		SafeRelease(td.second.pFontTexture);
		SafeRelease(td.second.pTextureView);
	}

	unordered_map<UINT, BillboardFontTextureData>().swap(umTextureData_);
}

//------------------------------------------------------------------------------
// 座標の設定
//------------------------------------------------------------------------------
void BillboardFont::SetPosition(const idea::Vector3D& position)
{
	position_ = position;

	billboard_.SetPosition(position_);
}

void BillboardFont::SetPosition(float positionX, float positionY, float positionZ)
{
	SetPosition(Vector3D(positionX, positionY, positionZ));
}

//------------------------------------------------------------------------------
// 色の設定
//------------------------------------------------------------------------------
void BillboardFont::SetColor(const idea::Color& color)
{
	color_.r = Clamp(color.r, 0.0f, 1.0f);
	color_.g = Clamp(color.g, 0.0f, 1.0f);
	color_.b = Clamp(color.b, 0.0f, 1.0f);
	color_.a = Clamp(color.a, 0.0f, 1.0f);

	billboard_.SetColor(color_);
}

void BillboardFont::SetColor(float r, float g, float b, float a)
{
	color_.r = Clamp(r, 0.0f, 1.0f);
	color_.g = Clamp(g, 0.0f, 1.0f);
	color_.b = Clamp(b, 0.0f, 1.0f);
	color_.a = Clamp(a, 0.0f, 1.0f);

	billboard_.SetColor(color_);
}

void BillboardFont::SetYBillboardFlag(bool flag)
{
	bYBillboard_ = flag;

	billboard_.SetYBillboardFlag(bYBillboard_);
}

void BillboardFont::DrawFormatText(const char* pFormat, ...)
{
	GraphicManager& gm = GraphicManager::Instance();

	// 準備ができていなければ終了
	if(hf_ == NULL
		|| !gm.GetContextPtr()){
		return;
	}

	vector<char> strBuffer;

	int width = 0;

	{
		va_list args;	// 可変個数の引数リスト

		// フォーマット指定された文字列を出力する
		va_start(args, pFormat);
		width = _vscprintf(pFormat, args) + 1;
		strBuffer.resize(width);
		vsprintf_s(&strBuffer[0], width, pFormat, args);
		va_end(args);
	}

	const char* str = &strBuffer[0];
	vector<char*> vecCharPtr;
	int len = StrSplit(str, &vecCharPtr);

	const char* pSrc = vecCharPtr[0];
	wchar_t wc[3] = { 0x00 };
	int ret = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, pSrc, 2, wc, 3);
	if(!ret){ return; }

	texture_.UnLoad();
	texture_.LoadImageFromTextureView(CreateTextureData((UINT)(*wc))->pTextureView);

	for(auto& cp : vecCharPtr){
		SafeDeleteArray(cp);
	}

	if(texture_.GetTextureViewPtr()){
		billboard_.SetTexture(texture_);
	} else{
		billboard_.SetTexture(pNullTexture_);
	}
}

BillboardFont::BillboardFontTextureData* BillboardFont::CreateTextureData(UINT code)
{
	if(HasKeyTextureData(code)){ return &umTextureData_[code]; }
	if(!hf_){ return &s_NullTextureData_; }

	HDC hdc = GetDC(NULL);
	HFONT oldFont = (HFONT)SelectObject(hdc, hf_);

	// 階調の最大値
	const int gradFlag = GGO_GRAY8_BITMAP;
	int grad = 0;
	switch(gradFlag)
	{
		case GGO_GRAY2_BITMAP:
			grad = 4;
			break;
		case GGO_GRAY4_BITMAP:
			grad = 16;
			break;
		case GGO_GRAY8_BITMAP:
			grad = 64;
			break;
	}

	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);

	GLYPHMETRICS glm;
	CONST MAT2 mat = { {0,1},{0,0},{0,0},{0,1} };
	DWORD size = GetGlyphOutlineW(hdc, code, gradFlag, &glm, 0, NULL, &mat);

	BYTE* pMono = new BYTE[size];
	GetGlyphOutlineW(hdc, code, gradFlag, &glm, size, pMono, &mat);

	SelectObject(hdc, oldFont);
	ReleaseDC(NULL, hdc);

	//フォントの幅と高さ
	int fontWidth = glm.gmCellIncX;
	int fontHeight = tm.tmHeight;

	ID3D11Texture2D* pFontTexture = nullptr;
	ID3D11ShaderResourceView* pTextureView = nullptr;

	//フォントを書き込むテクスチャ作成
	D3D11_TEXTURE2D_DESC fontTextureDesc;
	ZeroMemory(&fontTextureDesc, sizeof(fontTextureDesc));
	fontTextureDesc.Width = fontWidth;
	fontTextureDesc.Height = fontHeight;
	fontTextureDesc.MipLevels = 1;
	fontTextureDesc.ArraySize = 1;
	fontTextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	fontTextureDesc.SampleDesc.Count = 1;
	fontTextureDesc.SampleDesc.Quality = 0;
	fontTextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	fontTextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	fontTextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	fontTextureDesc.MiscFlags = 0;

	HRESULT hr = GraphicManager::Instance().GetDevicePtr()->CreateTexture2D(&fontTextureDesc, NULL, &pFontTexture);
	if(FAILED(hr)){ return &s_NullTextureData_; }

	// フォント情報をテクスチャに書き込む部分
	D3D11_MAPPED_SUBRESOURCE hMappedResource;
	hr = GraphicManager::Instance().GetContextPtr()->Map(
		pFontTexture,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&hMappedResource);
	if(FAILED(hr)){ return &s_NullTextureData_; }

	BYTE* pBits = (BYTE*)hMappedResource.pData;
	int ofsX = glm.gmptGlyphOrigin.x;
	int ofsY = tm.tmAscent - glm.gmptGlyphOrigin.y;
	int bmpW = (glm.gmBlackBoxX + 3) / 4 * 4;
	int bmpH = glm.gmBlackBoxY;
	int x, y;
	DWORD alpha, color;

	memset(pBits, 0, hMappedResource.RowPitch * tm.tmHeight);

	for(y = ofsY; y < ofsY + bmpH; ++y)
	{
		for(x = ofsX; x < ofsX + bmpW && x < fontWidth; ++x)
		{
			alpha = (255 * pMono[x - ofsX + bmpW * (y - ofsY)]) / grad;
			color = 0x00ffffff | (alpha << 24);
			memcpy((BYTE*)pBits + hMappedResource.RowPitch * y + 4 * x, &color, sizeof(DWORD));
		}
	}

	GraphicManager::Instance().GetContextPtr()->Unmap(pFontTexture, 0);

	SafeDeleteArray(pMono);

	//シェーダで使うためのビュー作成
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Format = fontTextureDesc.Format;
	viewDesc.Texture2D.MipLevels = fontTextureDesc.MipLevels;
	viewDesc.Texture2D.MostDetailedMip = 0;
	hr = GraphicManager::Instance().GetDevicePtr()->CreateShaderResourceView(pFontTexture, &viewDesc, &pTextureView);
	if(FAILED(hr)){ return &s_NullTextureData_; }

	if(code == L' ' || code == L'　'){
		SafeRelease(pFontTexture);
		SafeRelease(pTextureView);
	}

	BillboardFontTextureData td;
	ZeroMemory(&td, sizeof(td));
	td.pFontTexture = pFontTexture;
	td.pTextureView = pTextureView;
	td.fontWidth = fontWidth;
	td.fontHeight = fontHeight;

	umTextureData_.insert(pair<UINT, BillboardFontTextureData>{ code, td });

	return &umTextureData_[code];	// テクスチャーインターフェイスのポインターを返す
}

inline bool BillboardFont::HasKeyTextureData(UINT code)
{
	return (umTextureData_.find(code) != umTextureData_.end());
}