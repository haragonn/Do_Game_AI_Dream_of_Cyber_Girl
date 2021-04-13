/*==============================================================================
	[Actor2D.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_ACTOR2D_H
#define INCLUDE_IDEA_ACTOR2D_H

#include "../Utility/ideaMath.h"
#include "../Utility/ideaColor.h"

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Actor2D
	// クラス概要：2D上での位置と大きさ,角度を持つクラス
	// 　　　　　　コリジョンやフォロー機能を持つ
	//------------------------------------------------------------------------------
	class Actor2D{
	public:
		Actor2D();
		virtual ~Actor2D();

		// 初期化
		virtual void Init(const Vector2D& position, const Vector2D& size);

		// 初期化
		virtual void Init(float positionX, float positionY, const Vector2D& size);

		// 初期化
		virtual void Init(const Vector2D& position, float width, float height);

		// 初期化
		virtual void Init(float positionX, float positionY, float width, float height);

		// 終了処理
		virtual void UnInit();

		// 座標の設定
		virtual void SetPosition(const Vector2D& position);

		// 座標の設定
		virtual void SetPosition(float positionX, float positionY);

		// 前フレームの座標ごと座標を設定
		virtual void SetBasePosition(const Vector2D& position);

		// 前フレームの座標ごと座標を設定
		virtual void SetBasePosition(float positionX, float positionY);

		// 座標の移動
		virtual void MovePosition(const Vector2D& axis);

		// 座標の移動
		virtual void MovePosition(float axisX, float axisY);

		// 座標の取得
		inline const Vector2D& GetPosition()const
		{
			return position_;
		}

		// 前フレームの座標の取得
		inline const Vector2D& GetPrePosition()const
		{
			return prePosition_;
		}

		// 大きさの設定
		virtual void SetSize(const Vector2D& size);

		// 大きさの設定
		virtual void SetSize(float width, float height);

		// 大きさの取得
		inline const Vector2D& GetSize()const
		{
			return size_;
		}

		// 回転角の設定
		virtual void SetRotation(float radian);

		// 回転
		virtual void MoveRotation(float radian);

		// 回転角の取得
		inline float GetRoteate()const
		{
			return radian_;
		}

		// 色の設定
		virtual void SetColor(const Color& color);

		// 色の設定
		virtual void SetColor(float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);

		// 色の取得
		inline const Color& GetColor()const
		{
			return color_;
		}

		// 円の接触判定
		bool CheckHitCircle(const Actor2D& target)const;

		// 矩形の接触判定
		bool CheckHitRect(const Actor2D& target)const;

		// 位置関係取得
		float GetRelation(const Actor2D& target)const;

		// 前フレームの位置関係取得
		float GetPreFrameRelation(const Actor2D& target)const;

		// 追従するターゲットの設定
		virtual void FollowActor(Actor2D& target);

		// フォロワーをやめる
		virtual void QuitFollower();

	protected:
		// 座標
		Vector2D position_;

		// 前フレームの座標
		Vector2D prePosition_;

		// 大きさ
		Vector2D size_;

		// 角度
		float radian_ = 0.0f;

		// 反転フラグ(U)
		bool bReversedU_ = false;

		// 反転フラグ(V)
		bool bReversedV_ = false;

		// 色
		Color color_;

	private:
		friend class Actor2DManager;

		// 登録フラグ
		bool bRegistered_ = false;

		// リーダーのポインター
		Actor2D* pLeader_ = nullptr;

		// フォロワーのポインター
		Actor2D* pFollower_ = nullptr;

		// フォロワーの位置を移動
		void MoveFollower(Vector2D axis);

		// フォロワーの解散
		void ResetFollower(Actor2D* pTarget);

		// 座標の保持
		void UpdatePrePosition();
	};
}

#endif	// #ifndef INCLUDE_IDEA_ACTOR2D_H