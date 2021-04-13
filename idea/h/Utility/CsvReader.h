/*==============================================================================
	[CsvReader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CSVREADER_H
#define INCLUDE_IDEA_CSVREADER_H

//------------------------------------------------------------------------------
// �C���N���[�h�t�@�C��
//------------------------------------------------------------------------------
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <fstream>

#include "DynamicDualArray.h"


namespace idea{
	//------------------------------------------------------------------------------
	// �N���X���@�FCsvReader
	// �N���X�T�v�Fcsv�t�@�C���̓ǂݍ��݂��s���N���X
	//------------------------------------------------------------------------------
	class CsvReader{
	public:
		// Variant�\����
		// �����̌^�Ƃ��Ĉ�����\����
		struct Variant{
			// int�Ƃ��Ă̒l
			int asInt;

			// float�Ƃ��Ă̒l
			float asFloat;
			
			// std::string�Ƃ��Ă̒l
			std::string asString;

			Variant() :
				asInt(0),
				asFloat(0){}
			Variant(const std::string& str) :
				asInt(atoi(str.c_str())),
				asFloat((float)atof(str.c_str())),
				asString(str){}

			// int�Ƃ��Ď󂯎�����ꍇ
			operator int()
			{
				return asInt;
			}

			// float�Ƃ��Ď󂯎�����ꍇ
			operator float()
			{
				return asFloat;
			}

			// string�Ƃ��Ď󂯎�����ꍇ
			operator std::string()
			{
				return asString;
			}
		};

		// Record�N���X
		// csv�̃f�[�^��񕪂��i�[����N���X
		class Record{
		public:
			~Record();

			Variant& operator[](const char* pKey);
			Variant& operator[](int col);

			// �l�̒ǉ�
			void Add(const char* pKey, Variant& val);

		private:
			// �l
			std::unordered_map<std::string, Variant*> value_;
			
			// �L�[
			std::vector<std::string> key_;
		};

		CsvReader() :
			row_(0),
			col_(0){}
		CsvReader(const char* pFileName);

		~CsvReader();

		// csv�t�@�C���̓ǂݍ���
		bool LoadData(const char* pFileName);

		// csv�t�@�C���̓ǂݍ���
		void UnLoadData();

		// csv�t�@�C���̍s���̎擾
		int GetRow()const
		{
			return row_;
		}
		
		// csv�t�@�C���̗񐔂̎擾
		int GetCol()const
		{
			return col_;
		}

		// csv�t�@�C���̃t�B�[���h���擾
		Variant& GetField(const char* pID, const char* pKey);

		// csv�t�@�C���̃t�B�[���h���擾
		Variant& GetField(int row, const char* pKey);

		// csv�t�@�C���̃t�B�[���h���擾
		Variant& GetField(int row, int col);

		// csv�t�@�C���̃t�B�[���h���擾
		Record& operator[](const char* pID);
		Record& operator[](int row);

	private:
		// �s��
		int row_ = 0;
		
		// ��
		int col_ = 0;
		
		// ID���X�g
		std::vector<std::string> id_;
		
		// Record
		std::unordered_map<std::string, Record*> record_;
		
		// ��̒l
		static Variant NullValue;
		
		// ���Record
		static Record NullRecord;
	};

	class CsvWriter{
	public:
		template <typename T>
		static bool WriteData(const char* pFileName, DynamicDualArray<T> data);

		template <typename T>
		static bool WriteData(const char* pFileName, DynamicDualArray<T> data, const std::vector<const char*>& id, const std::vector<const char*>& key);
	};

	template<typename T>
	inline bool CsvWriter::WriteData(const char* pFileName, DynamicDualArray<T> data)
	{
		unsigned int row = data[0].size();

		if(!row ){
			return false;
		}

		unsigned int col = data.size() / row;

		if(!col){
			return false;
		}

		std::ofstream file(pFileName);

		for(unsigned int i = 0; i < col; ++i){
			for(unsigned int j = 0; j < row; ++j){
				file << data[i][j];

				if(j < row - 1){
					file << ",";
				}
			}

			if(i < col - 1){
				file << "\n";
			}
		}

		return true;
	}

	template<typename T>
	bool CsvWriter::WriteData(const char* pFileName, DynamicDualArray<T> data, const std::vector<const char*>& id, const std::vector<const char*>& key)
	{
		int row = data.GetRow();
		int col = data.GetCol();
		int idNum = id.size();
		int keyNum = key.size();

		if(!row || !col){
			return false;
		}

		if(idNum != row || keyNum != col){
			return false;
		}

		std::ofstream file(pFileName, std::ios::trunc);

		file << "id,";

		for(int i = 0; i < idNum; ++i){
			file << id[i];

			if(i < idNum - 1){
				file << ",";
			}
		}

		file << std::endl;

		for(int i = 0; i < col; ++i){
			file << key[i] << ",";

			for(int j = 0; j < row; ++j){
				file << data[i][j];

				if(j < row - 1){
					file << ",";
				}
			}

			if(i < col - 1){
				file << std::endl;
			}
		}

		return true;
	}
}

#endif	// #ifndef INCLUDE_IDEA_CSVREADER_H