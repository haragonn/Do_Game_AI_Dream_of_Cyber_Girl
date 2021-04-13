#ifndef INCLUDE_IDEA_WRAPCAMERA_H
#define INCLUDE_IDEA_WRAPCAMERA_H

#include "Camera.h"
#include "../../h/Utility/ideaMath.h"

namespace idea{
	class WrapCamera : public Camera{
	public:
		WrapCamera();
		~WrapCamera(){}

		// 初期化
		void Init(float viewAngle, float aspect, float nearZ, float farZ, float distance);

		// 注視点の設定
		void SetFocus(const Vector3D& position)override;

		// 注視点の設定
		void SetFocus(float positionX, float positionY, float positionZ)override;

		// 注視点の移動
		void MoveFocus(const Vector3D& axis)override;

		// 注視点の移動
		void MoveFocus(float axisX, float axisY, float axisZ)override;

		// 注視点との距離の設定
		void SetDistance(float distance);

		// 注視点との距離の移動
		void MoveDistance(float aixs);

		// 注視点との距離の取得
		float GetDistance()const{ return distance_; }

		// カメラアングルの設定
		void SetRotation(Vector2D rot);

		// カメラアングルの設定
		void SetRotation(float yaw, float pitch);

		// カメラアングルの移動
		void MoveRotation(Vector2D rot);

		// カメラアングルの移動
		void MoveRotation(float yaw, float pitch);

		// カメラアングルの移動
		void MoveRotation(Vector2D rot, float speed);

		// カメラアングルの移動
		void MoveRotation(float yaw, float pitch, float speed);

		// カメラアングルの取得
		float GetYaw(){ return rotation_.x; }

		// カメラアングルの取得
		float GetPitch(){ return rotation_.y; }

		// カメラアングルの閾値の設定
		void SetPitchThreshold(Vector2D pitch);

		// カメラアングルの閾値の設定
		void SetPitchThreshold(float pitchMin, float pithMax);

		// 補間目標カメラアングルの設定
		void SetWrapTarget(Vector2D rot);

		// 補間目標カメラアングルの設定
		void SetWrapTarget(float yaw, float pitch);

		// カメラアングルの補間
		void UpdateWrap(float delta);

		// 補間中か
		bool IsWrap()const{ return isWrap_; }

		// 補間の中止
		void InterruptionWrap();

		// カメラ視点からのベクトルの補正
		Vector2D FixVector(const Vector2D& vec);

	private:
		// 注視点からの距離
		float distance_ = 0.0f;

		// カメラアングル
		Vector2D rotation_;

		// カメラアングルの四元数
		Quaternion qtn_;

		// カメラアングルの閾値
		Vector2D pitchThreshold_;

		// 補間開始カメラアングル
		Quaternion startQtn_;

		// 補間目標カメラアングル
		Quaternion targetQtn_;

		// 補間中か
		bool isWrap_ = false;

		// 補間係数
		float t_ = 0.0f;

		// 座標の更新
		inline void UpdatePosition();

		// 初期化
		void Init(float viewAngle, float aspect, float nearZ, float farZ)override{}

		// 視点の設定
		void SetPosition(const Vector3D& position)override{}

		// 視点の設定
		void SetPosition(float positionX, float positionY, float positionZ)override{}

		// 視点の移動
		void MovePosition(const Vector3D& axis)override{}

		// 視点の移動
		void MovePosition(float axisX, float axisY, float axisZ)override{}
	};
}

#endif	// #ifndef INCLUDE_IDEA_WRAPCAMERA_H