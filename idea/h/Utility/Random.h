/*==============================================================================
	[Random.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_RANDOM_H
#define INCLUDE_IDEA_RANDOM_H

#include <vector>

namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FRandom
	// �N���X�T�v�F�����𐶐�����N���X
	//------------------------------------------------------------------------------
	class Random{
	public:
		Random();
		Random(unsigned long seed);

		// �V�[�h�l�̐ݒ�
		void SetSeed(unsigned long seed);

		// �V�[�h�l�̎擾
		unsigned long GetSeed()const{ return seed_; }

		// �����̎擾(0�`4,294,967,295)
		unsigned long GetRand();

		// �����̎擾(0�`�w�肵���ő�l)
		unsigned long GetRand(unsigned long max){ return GetRand() % (max + 1U); }

		// �����̎擾(0�`�w�肵���ő�l)
		unsigned long GetRand(unsigned long min, unsigned long max){ return (min < max) ? GetRand() % (max - min + 1U) + min : min; }

		// �����̎擾(���������_��)
		float GetRandF(float min, float max, unsigned long resolution = 4294967294U);

		// ���I���X�g�̃Z�b�g
		void SetList(unsigned long min, unsigned long max);

		// ���I���X�g������Ȃ������̎擾����
		unsigned long GetRandNotDuplicate();

		// ���I���X�g���󂩂ǂ���
		bool IsListEmpty(){ return !(vecList_.size()); }

	private:
		// �V�[�h�l
		unsigned long seed_ = 0U;

		// ���������p�s��
		unsigned long x_[4] = {};

		// ���I���X�g
		std::vector<unsigned long> vecList_;
	};
}

#endif // #ifndef INCLUDE_IDEA_RANDOM_H