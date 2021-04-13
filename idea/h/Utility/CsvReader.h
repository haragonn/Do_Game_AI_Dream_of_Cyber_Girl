/*==============================================================================
	[CsvReader.h]
														Author	:	Keigo Hara
==============================================================================*/
#ifndef INCLUDE_IDEA_CSVREADER_H
#define INCLUDE_IDEA_CSVREADER_H

//------------------------------------------------------------------------------
// インクルードファイル
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
	// クラス名　：CsvReader
	// クラス概要：csvファイルの読み込みを行うクラス
	//------------------------------------------------------------------------------
	class CsvReader{
	public:
		// Variant構造体
		// 複数の型として扱える構造体
		struct Variant{
			// intとしての値
			int asInt;

			// floatとしての値
			float asFloat;
			
			// std::stringとしての値
			std::string asString;

			Variant() :
				asInt(0),
				asFloat(0){}
			Variant(const std::string& str) :
				asInt(atoi(str.c_str())),
				asFloat((float)atof(str.c_str())),
				asString(str){}

			// intとして受け取った場合
			operator int()
			{
				return asInt;
			}

			// floatとして受け取った場合
			operator float()
			{
				return asFloat;
			}

			// stringとして受け取った場合
			operator std::string()
			{
				return asString;
			}
		};

		// Recordクラス
		// csvのデータ一列分を格納するクラス
		class Record{
		public:
			~Record();

			Variant& operator[](const char* pKey);
			Variant& operator[](int col);

			// 値の追加
			void Add(const char* pKey, Variant& val);

		private:
			// 値
			std::unordered_map<std::string, Variant*> value_;
			
			// キー
			std::vector<std::string> key_;
		};

		CsvReader() :
			row_(0),
			col_(0){}
		CsvReader(const char* pFileName);

		~CsvReader();

		// csvファイルの読み込み
		bool LoadData(const char* pFileName);

		// csvファイルの読み込み
		void UnLoadData();

		// csvファイルの行数の取得
		int GetRow()const
		{
			return row_;
		}
		
		// csvファイルの列数の取得
		int GetCol()const
		{
			return col_;
		}

		// csvファイルのフィールドを取得
		Variant& GetField(const char* pID, const char* pKey);

		// csvファイルのフィールドを取得
		Variant& GetField(int row, const char* pKey);

		// csvファイルのフィールドを取得
		Variant& GetField(int row, int col);

		// csvファイルのフィールドを取得
		Record& operator[](const char* pID);
		Record& operator[](int row);

	private:
		// 行数
		int row_ = 0;
		
		// 列数
		int col_ = 0;
		
		// IDリスト
		std::vector<std::string> id_;
		
		// Record
		std::unordered_map<std::string, Record*> record_;
		
		// 空の値
		static Variant NullValue;
		
		// 空のRecord
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