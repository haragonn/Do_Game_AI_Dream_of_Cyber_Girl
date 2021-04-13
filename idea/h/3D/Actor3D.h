/*==============================================================================
	[Actor3D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ACTOR3D_H
#define INCLUDE_IDEA_ACTOR3D_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"
#include "../3D/3DStruct.h"
#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Actor3D
	// クラス概要：3D上での座標,大きさ,回転,色を持つクラス
	//------------------------------------------------------------------------------
	class Actor3D{
	public:
		Actor3D();
		virtual ~Actor3D();

		// 初期化
		virtual void Init(const Vector3D& position);

		// 初期化
		virtual void Init(float positionX, float positionY, float positionZ);

		// 終了
		virtual void UnInit();

		// 座標の設定
		void SetPosition(const Vector3D& position);

		// 座標の設定
		void SetPosition(float positionX, float positionY, float positionZ);

		// 前フレームの座標ごと座標を設定
		void SetBasePosition(const Vector3D& position);

		// 前フレームの座標ごと座標を設定
		void SetBasePosition(float positionX, float positionY, float positionZ);

		// 座標の移動
		void MovePosition(const Vector3D& axis);

		// 座標の移動
		void MovePosition(float axisX, float axisY, float axisZ);

		// 座標の取得
		const Vector3D& GetPosition()const{ return position_; }

		// 回転角の設定
		void SetRotation(const Vector3D& rot);

		// 回転角の設定
		void SetRotation(float pitch, float yaw, float roll);

		// 回転
		void MoveRotation(const Vector3D& rot);

		// 回転
		void MoveRotation(float pitch, float yaw, float roll);

		// 回転角の取得
		const Vector3D& GetLocalRotation()const{ return rotation_; }

		// 回転行列の取得
		const Matrix4x4& GetRotationMatrix()const{ return rotationMatrix_; }

		// 拡縮倍率の設定
		void SetScale(const Vector3D& scale);

		// 拡縮倍率の設定
		void SetScale(float scaleX, float scaleY, float scaleZ);

		// 拡縮倍率の取得
		const Vector3D& GetScale()const{ return scale_; }

		// 包む球の半径の倍率の取得
		const float GetLengthRate()const{ return lengthRate_; }

		// 色の設定
		virtual void SetColor(const Color& color);

		// 色の設定
		virtual void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 色の設定
		void SetDiffuse(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 色の設定
		void SetDiffuse(const Color& color);

		// 色の設定
		void SetAmbient(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 色の設定
		void SetAmbient(const Color& color);

		// 色の設定
		void SetSpecular(const Color& color);

		// 色の設定
		void SetSpecular(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 光度の設定
		void SetShininess(float shininess);

		// 色の取得
		const Color& GetColor()const{ return color_; }

		// 子のセット
		void SetChild(Actor3D& child);

	protected:
		// 座標
		Vector3D position_;

		// 前フレームの座標
		Vector3D prePosition_;

		// 回転角
		Vector3D rotation_;

		// 拡縮倍率
		Vector3D scale_;

		// 包む球の半径の倍率
		float lengthRate_ = 0.0f;

		// 色
		Color color_;

		// マテリアル
		Material material_;

		// 回転行列
		Matrix4x4 rotationMatrix_;

		// ワールドマトリクス
		Matrix4x4 world_;

		// 子
		std::vector<Actor3D*> children_;

		// ワールドマトリクスの更新
		virtual inline void UpdateWorldMatrix();

		// ワールドマトリクスの更新
		virtual inline void UpdateWorldMatrix(Matrix4x4& parentMatrix);

	private:
		friend class Actor3DManager;

		// 登録フラグ
		bool bRegistered_ = false;

		// 座標の保持
		void UpdatePrePosition();
	};
}

#endif	// #ifndef INCLUDE_IDEA_ACTOR3D_H