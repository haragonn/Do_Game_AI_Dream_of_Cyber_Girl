/*==============================================================================
	[SpriteInstancing.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/2D/SpriteInstancing.h"
#include "../../h/Framework/GraphicManager.h"
#include "../../h/2D/SpriteInstancingManager.h"
#include "../../h/2D/SpriteManager.h"
#include "../../h/2D/Sprite.h"
#include "../../h/Texture/Texture.h"
#include "../../h/Utility/ideaMath.h"
#include "../../h/Utility/ideaType.h"
#include "../../h/Utility/ConstBufferType.h"

#include <algorithm>

//------------------------------------------------------------------------------
// using namespace
//------------------------------------------------------------------------------
using namespace std;
using namespace DirectX;
using namespace idea;


namespace{
//------------------------------------------------------------------------------
// �r���[�|�[�g�̐ݒ�
//------------------------------------------------------------------------------
	void SetViewPort()
	{
		GraphicManager& gm = GraphicManager::Instance();

		// �r���[�|�[�g�̐ݒ�
		D3D11_VIEWPORT viewPort;
		viewPort.TopLeftX = 0;
		viewPort.TopLeftY = 0;
		viewPort.Width = (FLOAT)gm.GetWidth();
		viewPort.Height = (FLOAT)gm.GetHeight();
		viewPort.MinDepth = 0.0f;
		viewPort.MaxDepth = 1.0f;

		gm.GetContextPtr()->RSSetViewports(1, &viewPort);
	}
}

//------------------------------------------------------------------------------
// �R���X�g���N�^
//------------------------------------------------------------------------------
SpriteInstancing::SpriteInstancing() :
	bReversedU_(false),
	bReversedV_(false),
	radian_(0.0f){}

//------------------------------------------------------------------------------
// �f�X�g���N�^
//------------------------------------------------------------------------------
SpriteInstancing::~SpriteInstancing()
{
	UnInit();
}

//------------------------------------------------------------------------------
// ������
//------------------------------------------------------------------------------
void SpriteInstancing::Init(const Vector2D& size)
{
	SetSize(size.x, size.y);
	radian_ = 0.0f;
	color_ = ideaColor::WHITE;
}

void SpriteInstancing::Init(float width, float height)
{
	SetSize(width, height);
	radian_ = 0.0f;
	color_ = ideaColor::WHITE;
}

//------------------------------------------------------------------------------
// �I������
//------------------------------------------------------------------------------
void SpriteInstancing::UnInit()
{
	size_ = Vector2D();
	radian_ = 0.0f;
	color_ = ideaColor::CLEAR;
	vector<Sprite*>().swap(vecSpritePtr_);
}

//------------------------------------------------------------------------------
// �傫���̐ݒ�
//------------------------------------------------------------------------------
void SpriteInstancing::SetSize(const Vector2D& size)
{
	size_ = size;

	// �l���}�C�i�X�̎��͔��]�t���O���I���ɂ��Đ����ɂ���
	bReversedU_ = false;

	if(size_.x < 0.0f){
		size_.x *= -1.0f;
		bReversedU_ = true;
	}

	bReversedV_ = false;

	if(size_.y < 0.0f){
		size_.y *= -1.0f;
		bReversedV_ = true;
	}
}

void SpriteInstancing::SetSize(float width, float height)
{
	SetSize(Vector2D(width, height));
}

//------------------------------------------------------------------------------
// ��]�p�̐ݒ�
//------------------------------------------------------------------------------
void SpriteInstancing::SetRotation(float rad)
{
	radian_ = WrapRadian(rad);
}

//------------------------------------------------------------------------------
// ��]
//------------------------------------------------------------------------------
void SpriteInstancing::MoveRotation(float rad)
{
	radian_ += rad;
	radian_ = WrapRadian(radian_);
}

//------------------------------------------------------------------------------
// �F�̐ݒ�
//------------------------------------------------------------------------------
void SpriteInstancing::SetColor(const Color& color)
{
	SetColor(color.r, color.g, color.b, color.a);
}

void SpriteInstancing::SetColor(float r, float g, float b, float a)
{
	color_.r = Clamp(r, 0.0f, 1.0f);
	color_.g = Clamp(g, 0.0f, 1.0f);
	color_.b = Clamp(b, 0.0f, 1.0f);
	color_.a = Clamp(a, 0.0f, 1.0f);
}

//------------------------------------------------------------------------------
// �`�悷��X�v���C�g�̒ǉ�
//------------------------------------------------------------------------------
void SpriteInstancing::AddSprite(Sprite& sprite)
{
	// ��ʊO�Ȃ�I��
	GraphicManager& gm = GraphicManager::Instance();

	// ��ʊO�Ȃ�I��
	float longVicinity = Max(size_.x, size_.y) * 0.5f * ideaMath::ROOT2;

	if(sprite.position_.x + longVicinity < 0.0f
		|| sprite.position_.x - longVicinity > gm.GetWidth()
		|| sprite.position_.y + longVicinity < 0.0f
		|| sprite.position_.y - longVicinity > gm.GetHeight()){
		return;
	}

	Sprite* pSpr = &sprite;
	auto it = find(vecSpritePtr_.begin(), vecSpritePtr_.end(), pSpr);

	if(it != vecSpritePtr_.end()){ return; }

	vecSpritePtr_.push_back(pSpr);

	auto itc = find(pSpr->vecSpriteInstancingPtr_.begin(), pSpr->vecSpriteInstancingPtr_.end(), this);

	if(itc == pSpr->vecSpriteInstancingPtr_.end()){
		pSpr->vecSpriteInstancingPtr_.push_back(this);
	}
}

//------------------------------------------------------------------------------
// �`�悷��X�v���C�g�̍폜
//------------------------------------------------------------------------------
void SpriteInstancing::RemoveSprite(Sprite& sprite)
{
	Sprite* pSpr = &sprite;
	auto it = find(vecSpritePtr_.begin(), vecSpritePtr_.end(), pSpr);

	if(it == vecSpritePtr_.end()){ return; }

	auto itc = find((*it)->vecSpriteInstancingPtr_.begin(), (*it)->vecSpriteInstancingPtr_.end(), this);
	if(itc == (*it)->vecSpriteInstancingPtr_.end()){ return; }
	(*itc) = nullptr;

	vecSpritePtr_.erase(it);
}

//------------------------------------------------------------------------------
// �`�悷��S�X�v���C�g���폜
//------------------------------------------------------------------------------
void SpriteInstancing::ResetSprite()
{
	if(vecSpritePtr_.size() > 0){
		for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd; ++it){
			auto itc = find((*it)->vecSpriteInstancingPtr_.begin(), (*it)->vecSpriteInstancingPtr_.end(), this);
			if(itc == (*it)->vecSpriteInstancingPtr_.end()){ continue; }
			(*itc) = nullptr;
		}
		vector<Sprite*>().swap(vecSpritePtr_);
	}
}

//------------------------------------------------------------------------------
// ��`�̕`��
//------------------------------------------------------------------------------
void SpriteInstancing::DrawRect(int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()
		|| vecSpritePtr_.size() == 0){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 0.0f) };
	vd[1] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 0.0f) };
	vd[2] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 1.0f) };
	vd[3] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 1.0f) };

	// ���_���̌v�Z
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;

	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->position_.x;
		pInstanceData[i].pos.y = (*it)->position_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(2, 1, &pSrv);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	SetViewPort();

	//�|���S���`��
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}

//------------------------------------------------------------------------------
// �e�N�X�`���̕`��
//------------------------------------------------------------------------------
void SpriteInstancing::DrawTexture(const Texture& tex, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()
		|| vecSpritePtr_.size() == 0){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 0.0f) };
	vd[1] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 0.0f) };
	vd[2] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 1.0f) };
	vd[3] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 1.0f) };

	// ���_���̌v�Z
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;

	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? 1.0f : 0.0f;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? 1.0f : 0.0f;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->position_.x;
		pInstanceData[i].pos.y = (*it)->position_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(2, 1, &pSrv);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	SetViewPort();

	//�|���S���`��
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}

//------------------------------------------------------------------------------
// ���������ꂽ�e�N�X�`���̕`��
//------------------------------------------------------------------------------
void SpriteInstancing::DrawDividedTexture(const Texture& tex, int uNum, int vNum, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()
		|| vecSpritePtr_.size() == 0){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 0.0f) };
	vd[1] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 0.0f) };
	vd[2] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 1.0f) };
	vd[3] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 1.0f) };

	// ���_���̌v�Z
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;

	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	float u1 = tex.GetDivU() * uNum;
	float u2 = tex.GetDivU() * (uNum + 1);
	float v1 = tex.GetDivV() * vNum;
	float v2 = tex.GetDivV() * (vNum + 1);

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? v2 : v1;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->position_.x;
		pInstanceData[i].pos.y = (*it)->position_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(2, 1, &pSrv);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	SetViewPort();

	//�|���S���`��
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}

//------------------------------------------------------------------------------
// �\���͈͂��w�肵���e�N�X�`���[�̕`��
//------------------------------------------------------------------------------
void SpriteInstancing::DrawDelimitedTexture(const Texture& tex, float u, float v, float width, float height, int blend)
{
	// �������ł��Ă��Ȃ���ΏI��
	GraphicManager& gm = GraphicManager::Instance();
	SpriteManager& sm = SpriteManager::Instance();
	SpriteInstancingManager& sim = SpriteInstancingManager::Instance();

	if(!gm.GetContextPtr()
		|| !sm.GetPixelShederDefaultPtr()
		|| !sm.GetPixelShederTexturePtr()
		|| !sm.GetRectVertexBufferPtr()
		|| !sim.GetVertexShederPtr()){
		return;
	}

	// ���_���
	VertexData2D vd[SpriteManager::RECT_VERTEX_NUM];
	vd[0] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 0.0f) };
	vd[1] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 0.0f) };
	vd[2] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(0.0f, 1.0f) };
	vd[3] = { Vector3D(0.0f, 0.0f, 0.0f), color_, Vector2D(1.0f, 1.0f) };

	// ���_���̌v�Z
	float halfWidth = size_.x * 0.5f;
	float halfHeight = size_.y * 0.5f;

	float x, y, axisX, axisY;

	float tSin = sinf(radian_);
	float tCos = cosf(radian_);

	float u1 = u;
	float u2 = u + width;
	float v1 = v;
	float v2 = v + height;

	for(int i = SpriteManager::RECT_VERTEX_NUM - 1; i >= 0; --i){
		x = (i % 2) ? halfWidth : -halfWidth;
		y = (i > 1) ? halfHeight : -halfHeight;
		axisX = x * tCos - y * tSin;
		axisY = x * tSin + y * tCos;
		vd[i].pos.x = axisX;
		vd[i].pos.y = axisY;
		vd[i].tex.x = (!!(i % 2) ^ bReversedU_) ? u2 : u1;
		vd[i].tex.y = (!!(i > 1) ^ bReversedV_) ? v2 : v1;
	}

	// �o�b�t�@��������
	D3D11_MAPPED_SUBRESOURCE msr;
	ID3D11Buffer* pVBuf = sm.GetRectVertexBufferPtr();

	gm.GetContextPtr()->Map(pVBuf, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);
	memcpy(msr.pData, vd, sizeof(VertexData2D) * SpriteManager::RECT_VERTEX_NUM);
	gm.GetContextPtr()->Unmap(pVBuf, 0);

	// ���_�o�b�t�@�̃Z�b�g
	UINT stride = sizeof(VertexData2D);
	UINT offset = 0;

	gm.GetContextPtr()->IASetVertexBuffers(0, 1, &pVBuf, &stride, &offset);

	// �e�N�X�`����������
	ID3D11ShaderResourceView* pTexView = tex.GetTextureViewPtr();

	if(pTexView){
		gm.GetContextPtr()->PSSetShaderResources(0, 1, &pTexView);
	}

	// ���̓��C�A�E�g�̃Z�b�g
	gm.GetContextPtr()->IASetInputLayout(sm.GetInputLayoutPtr());

	// �v���~�e�B�u�`��̃Z�b�g
	gm.GetContextPtr()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	gm.GetContextPtr()->Map(sim.GetInstanceDataBufferPtr(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);

	PerInstanceData* pInstanceData = (PerInstanceData*)MappedResource.pData;

	int i = 0;
	for(auto it = begin(vecSpritePtr_), itEnd = end(vecSpritePtr_); it != itEnd && i < SpriteInstancingManager::INSTANCE_MAX; ++i, ++it){
		pInstanceData[i].pos.x = (*it)->position_.x;
		pInstanceData[i].pos.y = (*it)->position_.y;
		pInstanceData[i].pos.z = 0.0f;
		pInstanceData[i].pos.w = 1.0f;
	}

	gm.GetContextPtr()->Unmap(sim.GetInstanceDataBufferPtr(), 0);

	ID3D11ShaderResourceView* pSrv = sim.GetTransformShaderResourceViewPtr();
	gm.GetContextPtr()->VSSetShaderResources(2, 1, &pSrv);

	// �u�����f�B���O�̃Z�b�g
	if(!blend){
		gm.SetBlendState(BLEND_STATE::BLEND_ALIGNMENT);
	} else if(blend > 0){
		gm.SetBlendState(BLEND_STATE::BLEND_ADD);
	} else{
		gm.SetBlendState(BLEND_STATE::BLEND_SUBTRACT);
	}

	// �V�F�[�_�̃Z�b�g
	gm.GetContextPtr()->VSSetShader(sim.GetVertexShederPtr(), NULL, 0);
	gm.GetContextPtr()->HSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->DSSetShader(NULL, NULL, 0);
	gm.GetContextPtr()->GSSetShader(NULL, NULL, 0);
	if(pTexView){
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederTexturePtr(), NULL, 0);
	} else{
		gm.GetContextPtr()->PSSetShader(sm.GetPixelShederDefaultPtr(), NULL, 0);
	}
	gm.GetContextPtr()->CSSetShader(NULL, NULL, 0);

	// �r���[�|�[�g�̐ݒ�
	SetViewPort();

	//�|���S���`��
	gm.GetContextPtr()->DrawInstanced(SpriteManager::RECT_VERTEX_NUM, min(SpriteInstancingManager::INSTANCE_MAX, vecSpritePtr_.size()), 0, 0);
}
