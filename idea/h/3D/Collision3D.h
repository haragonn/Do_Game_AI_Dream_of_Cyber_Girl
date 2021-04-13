#ifndef INCLUDE_IDEA_COLLISION_H
#define INCLUDE_IDEA_COLLISION_H

#include "../../h/Utility/ideaMath.h"

namespace idea{
	// 前方宣言
	class BoxCollider;
	class SphereCollider;
	class CapsuleCollider;

	class BaseCollider{
	public:
		// ボックスコライダーとの接触判定
		virtual bool CheckHit(const BoxCollider& bc) = 0;

		// スフィアコライダーとの接触判定
		virtual bool CheckHit(const SphereCollider& sc) = 0;

		// カプセルコライダーとの接触判定
		virtual bool CheckHit(const CapsuleCollider& cc) = 0;

		// 点との最接近点を取得する
		virtual Vector3D GetClosetPoint(const Vector3D& pt) = 0;

		// めり込んだボックスコライダーから離れる
		virtual Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f) = 0;

		// めり込んだスフィアコライダーから離れる
		virtual Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f) = 0;

		// めり込んだカプセルコライダーから離れる
		virtual Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f) = 0;
	};

	// ボックスコライダー(OBB)
	class BoxCollider : public BaseCollider{
	public:
		// OBBの設定
		void SetOBB(const Matrix4x4& world, const Vector3D& prePosition);

		// ボックスコライダーとの接触判定
		bool CheckHit(const BoxCollider& bc)override;

		// スフィアコライダーとの接触判定
		bool CheckHit(const SphereCollider& sc)override;

		// カプセルコライダーとの接触判定
		bool CheckHit(const CapsuleCollider& cc)override;

		// 点との最接近点を取得する
		Vector3D GetClosetPoint(const Vector3D& pt)override;

		// めり込んだボックスコライダーから離れる
		Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f)override;

		// めり込んだスフィアコライダーから離れる
		Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f)override;

		// めり込んだカプセルコライダーから離れる
		Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f)override;

		// 位置の取得
		const Vector3D& GetPosition()const{ return obb_.c; }

	private:
		friend class Collision3DHelper;

		// OBB
		struct OBB{
			Vector3D c;
			Vector3D u[3];
			Vector3D e;
		}obb_;

		// 前の位置
		Vector3D prePosition_;
	};

	// スフィアコライダー
	class SphereCollider : public BaseCollider{
	public:
		SphereCollider() : radius_(0.0f){}

		// スフィアコライダーの設定
		void SetSphere(const Vector3D& position, float radius, const Vector3D& prePosition);

		// ボックスコライダーとの接触判定
		bool CheckHit(const BoxCollider& bc)override;

		// スフィアコライダーとの接触判定
		bool CheckHit(const SphereCollider& sc)override;

		// カプセルコライダーとの接触判定
		bool CheckHit(const CapsuleCollider& cc)override;

		// 点との最接近点を取得する
		Vector3D GetClosetPoint(const Vector3D& pt)override;

		// めり込んだボックスコライダーから離れる
		Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f)override;

		// めり込んだスフィアコライダーから離れる
		Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f)override;

		// めり込んだカプセルコライダーから離れる
		Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f)override;

		// 位置の取得
		const Vector3D& GetPosition()const{ return position_; }

		// 半径の取得
		float GetRadius()const{ return radius_; }

	private:
		friend class Collision3DHelper;

		// 位置
		Vector3D position_;

		// 前の位置
		Vector3D prePosition_;

		// 半径
		float radius_ = 0.0f;
	};

	// カプセルコライダー
	class CapsuleCollider : public BaseCollider{
	public:
		CapsuleCollider() : radius_(0.0f), height_(0.0f){}

		// カプセルコライダーの設定
		void SetCapsule(const Matrix4x4& world, float radius, float height, const Vector3D& prePosition);

		// ボックスコライダーとの接触判定
		bool CheckHit(const BoxCollider& bc)override;

		// スフィアコライダーとの接触判定
		bool CheckHit(const SphereCollider& sc)override;

		// カプセルコライダーとの接触判定
		bool CheckHit(const CapsuleCollider& cc)override;

		// 点との最接近点を取得する
		Vector3D GetClosetPoint(const Vector3D& pt)override;

		// めり込んだボックスコライダーから離れる
		Vector3D CollisionEscape(const BoxCollider& bc, float grip = 0.0f)override;

		// めり込んだスフィアコライダーから離れる
		Vector3D CollisionEscape(const SphereCollider& sc, float grip = 0.0f)override;

		// めり込んだカプセルコライダーから離れる
		Vector3D CollisionEscape(const CapsuleCollider& cc, float grip = 0.0f)override;

		// 位置の取得
		const Vector3D& GetPosition()const{ return position_; }

		// 半径の取得
		float GetRadius()const{ return radius_; }

		// 高さの取得
		float GetHeight()const{ return height_; }

		// 上方の取得
		const Vector3D& GetTop()const{ return top_; }

		// 下方の取得
		const Vector3D& GetBottom()const{ return bottom_; }

	private:
		friend class Collision3DHelper;

		// 位置
		Vector3D position_;

		// 前の位置
		Vector3D prePosition_;

		// 上方
		Vector3D top_;

		// 下方
		Vector3D bottom_;

		// 回転行列
		Matrix4x4 world_;

		// 半径
		float radius_ = 0.0f;

		// 高さ
		float height_ = 0.0f;
	};
}

#endif	// #ifndef INCLUDE_IDEA_COLLISION_H