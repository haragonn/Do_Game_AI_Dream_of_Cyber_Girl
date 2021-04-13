/*==============================================================================
	[Random.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_RANDOM_H
#define INCLUDE_IDEA_RANDOM_H

#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// クラス名　：Random
	// クラス概要：乱数を生成するクラス
	//------------------------------------------------------------------------------
	class Random{
	public:
		Random();
		Random(unsigned long seed);

		// シード値の設定
		void SetSeed(unsigned long seed);

		// シード値の取得
		unsigned long GetSeed()const{ return seed_; }

		// 乱数の取得(0～4,294,967,295)
		unsigned long GetRand();

		// 乱数の取得(0～指定した最大値)
		unsigned long GetRand(unsigned long max){ return GetRand() % (max + 1U); }

		// 乱数の取得(0～指定した最大値)
		unsigned long GetRand(unsigned long min, unsigned long max){ return (min < max) ? GetRand() % (max - min + 1U) + min : min; }

		// 乱数の取得(浮動小数点数)
		float GetRandF(float min, float max, unsigned long resolution = 4294967294U);

		// 抽選リストのセット
		void SetList(unsigned long min, unsigned long max);

		// 抽選リストから被りなし乱数の取得する
		unsigned long GetRandNotDuplicate();

		// 抽選リストが空かどうか
		bool IsListEmpty(){ return !(vecList_.size()); }

	private:
		// シード値
		unsigned long seed_ = 0U;

		// 乱数発生用行列
		unsigned long x_[4] = {};

		// 抽選リスト
		std::vector<unsigned long> vecList_;
	};
}

#endif // #ifndef INCLUDE_IDEA_RANDOM_H