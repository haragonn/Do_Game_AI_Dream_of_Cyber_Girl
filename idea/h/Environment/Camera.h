#ifndef INCLUDE_IDEA_CAMERA_H
#define INCLUDE_IDEA_CAMERA_H

//------------------------------------------------------------------------------
// インクルードファイル
//------------------------------------------------------------------------------
#include "../../h/3D/Collision3D.h"
#include "../../h/Utility/ideaMath.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// 前方宣言
	//------------------------------------------------------------------------------
	class Object;

	//------------------------------------------------------------------------------
	// クラス名　：Camera
	// クラス概要：3Dオブジェクトを描画する為のカメラのクラス
	//------------------------------------------------------------------------------
	class Camera{
	public:
		// ビューポート
		struct ViewPort{
			float topLeftX;
			float topLeftY;
			float width;
			float height;
			float minDepth;
			float maxDepth;
		};

		Camera();
		virtual ~Camera();

		// 初期化
		virtual void Init(float viewAngle, float aspect, float nearZ, float farZ);

		// 視野角の設定
		virtual void SetViewAngle(float viewAngle);

		// 視点の設定
		virtual void SetPosition(const Vector3D& position);

		// 視点の設定
		virtual void SetPosition(float positionX, float positionY, float positionZ);

		// 視点の移動
		virtual void MovePosition(const Vector3D& axis);

		// 視点の移動
		virtual void MovePosition(float axisX, float axisY, float axisZ);

		// 視点の取得
		const Vector3D& GetPosition()const{ return eye_; }

		// 注視点の設定
		virtual void SetFocus(const Vector3D& position);

		// 注視点の設定
		virtual void SetFocus(float positionX, float positionY, float positionZ);

		// 注視点の移動
		virtual void MoveFocus(const Vector3D& axis);

		// 注視点の移動
		virtual void MoveFocus(float axisX, float axisY, float axisZ);

		// 注視点の取得
		const Vector3D& GetFocus()const{ return focus_; }

		// ビューポートの設定
		void SetViewPort(float topLeftX, float topLeftY, float width, float height);

		// 被写体の登録
		void AddObject(Object& object);

		// 被写体の登録解除
		void RemoveObject(Object& object);

		// 全被写体の登録解除
		void ResetObject();

		// 被写体の描画
		void DrawObject();

		// コライダーの半径の設定
		void SetColliderRadius(float radius){ radius_ = radius; }

		// スフィアコライダーの取得
		SphereCollider GetCollider(){ return sc_; }

		// カメラに写っているか
		bool IsVisible(const Object& object);

		// 視点と注視点の間に存在するか
		bool CheckHitRayToSphere(const Object& object);

		// プロジェクションマトリクスの取得
		const Matrix4x4& GetProjectionMatrix()const{ return proj_; }

		// ビューマトリクスの取得
		const Matrix4x4& GetViewMatrix()const{ return view_; }

		// ビューポートの取得
		const ViewPort& GetViewPort()const{ return viewPort_; }

	protected:
		// 視野角
		float viewAngle_ = 0.0f;

		// アスペクト
		float aspect_ = 0.0f;

		// ニア
		float nearZ_ = 0.0f;

		// ファー
		float farZ_ = 0.0f;

		// 視点
		Vector3D eye_;

		// 注視点
		Vector3D focus_;

		// カメラの上ベクトル
		Vector3D up_;

		// スフィアコライダー
		SphereCollider sc_;

		// プロジェクションマトリクス
		Matrix4x4 proj_;

		// ビューマトリクス
		Matrix4x4 view_;

		// ビューポート
		ViewPort viewPort_;

		// コリジョンの半径
		float radius_ = 0.0f;


		// 被写体リスト
		std::vector<Object*> vecObjPtr_;

		// ビューマトリクスの更新
		inline void UpdateViewMatrix();
	};
}

#endif	// #ifndef INCLUDE_IDEA_CAMERA_H