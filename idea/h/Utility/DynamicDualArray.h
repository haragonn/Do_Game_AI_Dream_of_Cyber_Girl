#ifndef INCLUDE_IDEA_DYNAMICDUALARRAY_H
#define INCLUDE_IDEA_DYNAMICDUALARRAY_H

#include <unordered_map>

namespace idea{
	template<typename T>
	class DynamicDualArray{
	private:
		class DynamicArray{
		public:
			T& operator[](int col)
			{
				return value_[col];
			}

			int GetSize()
			{
				return value_.size();
			}

		private:
			std::unordered_map<unsigned int, T> value_;
		};

		std::unordered_map<unsigned int, DynamicArray> map_;

	public:
		DynamicDualArray(){}

		DynamicDualArray(int row, int col, T val)
		{
			Reserve(row, col, val);
		}

		DynamicArray& operator[](int row)
		{
			return map_[row];
		}

		void Clear()
		{
			std::unordered_map<unsigned int, DynamicArray>().swap(map_);
		}

		void Reserve(int row, int col, T val)
		{
			for(int i = row - 1; i >= 0; --i){
				for(int j = col - 1; j >= 0; --j){
					(*this)[i][j] = val;
				}
			}
		}

		int GetSize()
		{
			int ret = 0;

			for(auto& m : map_)
			{
				ret += m.second.GetSize();
			}

			return ret;
		}

		int GetRow()
		{
			return static_cast<int>(map_[0].GetSize());
		}

		int GetCol()
		{
			int row = GetRow();

			if(!row){
				return 0;
			}

			return static_cast<int>(GetSize() / row);
		}
	};
}

#endif	// #ifndef INCLUDE_IDEA_DYNAMICDUALARRAY_H