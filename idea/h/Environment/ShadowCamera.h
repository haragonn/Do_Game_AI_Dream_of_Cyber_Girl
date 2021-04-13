#ifndef INCLUDE_IDEA_SHADOWCAMERA_H
#define INCLUDE_IDEA_SHADOWCAMERA_H

#include "Camera.h"

namespace idea{
	class ShadowCamera : public Camera{
	public:
		ShadowCamera() : bShadowDraw_(false){}

		// 初期化
		void Init(float range, float nearZ, float farZ);

		// 視点の設定
		void SetPosition(const Vector3D& position)override;

		// 視点の設定
		void SetPosition(float positionX, float positionY, float positionZ)override;

		// 視点の移動
		void MovePosition(const Vector3D& axis)override;

		// 視点の移動
		void MovePosition(float axisX, float axisY, float axisZ)override;

		// 注視点の設定
		virtual void SetFocus(const Vector3D& position)override{}

		// 注視点の設定
		virtual void SetFocus(float positionX, float positionY, float positionZ)override{}

		// 注視点の移動
		virtual void MoveFocus(const Vector3D& axis)override{}

		// 注視点の移動
		virtual void MoveFocus(float axisX, float axisY, float axisZ)override{}

		// 被写体の描画
		void DrawObject();

		// 影描写中フラグの取得
		bool GetShadowDrawFlag()const{ return bShadowDraw_; }

	private:
		// 座標
		Vector3D position_;

		// 影描画フラグ
		bool bShadowDraw_ = false;

		// 初期化
		void Init(float viewAngle, float aspect, float nearZ, float farZ)override{}
	};
}

#endif	// #ifndef INCLUDE_IDEA_SHADOWCAMERA_H