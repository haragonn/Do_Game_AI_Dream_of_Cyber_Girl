#ifndef INCLUDE_IDEA_GAMEOBJECT_H
#define INCLUDE_IDEA_GAMEOBJECT_H

#include "../Utility/ideaMath.h"
#include "../Framework/GameManager.h"

namespace idea{
	class GameObject{
	public:
		GameObject() :
			bDestroy_(false)
		{
			GameManager gameManager;

			pScene_ = gameManager.GetScenePtr();
		}

		virtual ~GameObject() = default;

		// 初期化
		virtual void Init() = 0;

		// 終了処理
		virtual void UnInit()
		{}

		// 更新
		virtual void Update()
		{}

		// 描画
		virtual void Draw()
		{}

		// 破棄
		inline void SetDestroy()
		{
			bDestroy_ = true;
		}

		// 座標の設定
		inline void SetPosition(const Vector3D& position)
		{
			position_ = position;
		}

		// 座標の設定
		inline void SetPosition(float positionX, float positionY, float positionZ)
		{
			position_ = Vector3D(positionX, positionY, positionZ);
		}

		// 座標の移動
		inline void MovePosition(const Vector3D& axis)
		{
			position_ += axis;
		}

		// 座標の移動
		inline void MovePosition(float axisX, float axisY, float axisZ)
		{
			position_.x += axisX;
			position_.y += axisY;
			position_.z += axisZ;
		}

		// 座標の取得
		inline const Vector3D& GetPosition()const
		{
			return position_;
		}

		// 回転角の設定
		inline void SetRotation(const Vector3D& rotation)
		{
			rotation_ = rotation;
		}

		// 回転角の設定
		inline void SetRotation(float pitch, float yaw, float roll)
		{
			rotation_.x = pitch;
			rotation_.y = yaw;
			rotation_.z = roll;
		}

		// 回転
		inline void MoveRotation(const Vector3D& rotation)
		{
			rotation_ += rotation;
		}

		// 回転
		inline void MoveRotation(float pitch, float yaw, float roll)
		{
			rotation_.x += pitch;
			rotation_.y += yaw;
			rotation_.z += roll;
		}

		// 回転角の取得
		inline const Vector3D& GetRotation()const
		{
			return rotation_;
		}

		// 拡縮倍率の設定
		inline void SetScale(const Vector3D& scale)
		{
			scale_ = scale;
		}

		// 拡縮倍率の設定
		inline void SetScale(float scaleX, float scaleY, float scaleZ)
		{
			scale_.x = scaleX;
			scale_.y = scaleY;
			scale_.z = scaleZ;
		}

		// 拡縮倍率の取得
		inline const Vector3D& GetScale()const
		{
			return scale_;
		}

		// 前方ベクトルの取得
		inline Vector3D GetForward()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._31, rot._32, rot._33);
		}

		// 右ベクトルの取得
		inline Vector3D GetRight()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._11, rot._12, rot._13);
		}

		// 上ベクトルの取得
		inline Vector3D GetUp()
		{
			Matrix4x4 rot = Matrix4x4Rotation(rotation_);

			return Vector3D(rot._21, rot._22, rot._23);
		}

	protected:
		// 座標
		Vector3D position_;

		// 回転角
		Vector3D rotation_;

		// 拡縮
		Vector3D scale_ = { 1.0f, 1.0f, 1.0f };

		// 現在のシーンのポインタ
		Scene* pScene_ = nullptr;

		// 破棄フラグ
		bool bDestroy_ = false;

	private:
		friend class Scene;

		// 破棄の実行
		bool Destroy()
		{
			if(!bDestroy_){
				return false;
			}

			UnInit();

			delete this;

			return true;
		}
	};
}

#endif	// #ifndef INCLUDE_IDEA_GAMEOBJECT_H