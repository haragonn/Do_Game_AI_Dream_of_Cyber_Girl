/*==============================================================================
	[CsvReader.cpp]
														Author	:	Keigo Hara
==============================================================================*/
#include "../../h/Utility/CsvReader.h"
#include "../../h/Utility/ideaUtility.h"

using namespace std;
using namespace idea;

namespace{
	const int MAX_READ_LINE = 2048;	// ��s�̃f�[�^�Ƃ��ēǂݍ��߂�ő啶����
}

// ���݂��Ȃ����̂��w�肳�ꂽ���ɕԂ�����
CsvReader::Variant CsvReader::NullValue;
CsvReader::Record CsvReader::NullRecord;

// int����string�֕ϊ�
string IntToString(int val)
{
	stringstream ss;
	ss << val;
	return ss.str();
}

// Context�N���X
// csv�̓ǂݍ��ݕ������߂�N���X
template<int N>class Context{
public:
	char str_[N] = {};

private:
	int size_ = 0;
	int position_ = 0;
	bool eol_ = false;

public:
	Context() :
		size_(N),
		position_(0),
		eol_(false){}

	// ���ڂ���ǂݐi�߂�
	string GetItem()
	{
		char c[N] = {};
		int j = 0;

		for(int i = position_; i < size_; ++i, ++j, ++position_){
			switch(str_[i]){
				// ���ڂ̋�؂�
				case ',':
					++position_;
					return c;
					// �s�̏I���
				case '\0':
					++position_;
					eol_ = true;
					return c;
					// �������镶��
				case ' ':
				case '\t':
					--j;
					break;
				default:
					c[j] = str_[i];
			}
		}

		eol_ = true;

		return c;
	}

	// �ǂވʒu�̃��Z�b�g
	void Reset()
	{
		position_ = 0;
		eol_ = false;
	}

	// ��s�ǂݏI��������ǂ���
	bool EOL()
	{
		if(eol_){
			eol_ = false;
			return true;
		}

		return false;
	}
};

// Record�N���X
// csv�̃f�[�^��񕪂��i�[����N���X
CsvReader::Record::~Record()
{
	for(auto& v : value_){
		delete v.second;
	}
}

CsvReader::Variant& CsvReader::Record::operator[](const char* pKey)
{
	// �w�肳�ꂽKey�����݂���΂��̓��e��Ԃ�
	if(value_.find(pKey) == value_.end()){
		return NullValue;
	}

	return *value_[pKey];
}

CsvReader::Variant& CsvReader::Record::operator[](int col)
{
	// �ʂ��ԍ�����ID������o���Ă��̓��e��Ԃ�
	if((int)key_.size() > col && col > -1){
		return *value_[key_[col]];
	} else{
		return NullValue;
	}
}

void CsvReader::Record::Add(const char* pKey, Variant& val)
{
	// �w�肳�ꂽKey�����݂��Ȃ���ΐV�����쐬����
	if(value_.find(pKey) == value_.end()){
		value_[pKey] = new Variant(val);
		key_.push_back(pKey);
	} else{
		(*value_[pKey]) = val;
	}
}

// CsvReader�N���X
// �����s��Record����Ȃ�csv�̃f�[�^���i�[����N���X
CsvReader::CsvReader(const char* pFileName)
{
	LoadData(pFileName);
}

CsvReader::~CsvReader()
{
	for(auto& r : record_){
		delete r.second;
	}
}

bool CsvReader::LoadData(const char* pFileName)
{
	ifstream ifs(pFileName);

	if(ifs.is_open() == true){
		// �f�[�^Key���̎擾
		vector<string> key;
		Context<MAX_READ_LINE> ct;
		string tStr;
		string tID;
		bool bID = false;

		do{
			ifs.getline(ct.str_, MAX_READ_LINE - 1);
			tStr = ct.str_;
		} while(tStr.empty() && !ifs.eof());

		col_ = 0;
		while(!ct.EOL()){
			key.push_back(ct.GetItem());
			++col_;
		}
		ct.Reset();

		// ID�̎g�p�m�F
		if(key[0] == "id"
			|| key[0] == "Id"
			|| key[0] == "ID"){
			bID = true;
		} else{
			ifs.seekg(0, ios_base::beg);
		}

		// �f�[�^�擾����ъi�[
		row_ = 0;
		while(!ifs.eof()){
			ifs.getline(ct.str_, MAX_READ_LINE - 1);
			tStr = ct.str_;
			if(!tStr.empty()){
				Record* tRecord = new Record();
				// ID�̎g�p�m�F
				if(bID){
					tID = ct.GetItem();
					id_.push_back(tID);
				} else{
					tID = IntToString(row_);
					id_.push_back(tID);
				}
				int cnt = 0;
				for(int i = 0; i < col_ && !ct.EOL(); ++i){
					if(i == 0 && bID){
						continue;
					}

					Variant val = Variant(ct.GetItem());

					if(!bID){
						++cnt;
						ostringstream oss;
						oss << cnt;

						tRecord->Add(oss.str().c_str(), val);
					} else{
						tRecord->Add(key[i].c_str(), val);
					}
				}

				record_[tID] = tRecord;

				++row_;
			}

			ct.Reset();
		}

		if(bID){
			--col_;
		}

		return true;
	}

	return false;
}

void CsvReader::UnLoadData()
{
	if(record_.size() == 0){
		return;
	}

	for(auto& r : record_){
		delete r.second;
	}

	std::unordered_map<string, Record*>().swap(record_);

	vector<string>().swap(id_);
}

CsvReader::Variant& CsvReader::GetField(const char* pID, const char* pKey)
{
	// �w�肳�ꂽID�����݂���΂��̓��e��Ԃ�,Key�̔����operator[](pKey)�ɔC����
	if(record_.find(pID) == record_.end()){
		return NullValue;
	}

	return record_[pID]->operator[](pKey);
}

CsvReader::Variant& CsvReader::GetField(int row, const char* pKey)
{
	// �ʂ��ԍ�����ID������o��
	if((int)id_.size() > row && row >= 0){
		// �w�肳�ꂽID�̓��e��Ԃ�,pKey�̔����operator[](pKey)�ɔC����
		return record_[id_[row]]->operator[](pKey);
	}

	return NullValue;
}

CsvReader::Variant& CsvReader::GetField(int row, int col)
{
	// �ʂ��ԍ�����ID������o��
	if((int)id_.size() > row && row >= 0){
		// �w�肳�ꂽID,Key�̓��e��Ԃ�
		return record_[id_[row]]->operator[](col);
	}

	return NullValue;
}

CsvReader::Record& CsvReader::operator[](const char* pID)
{
	// �w�肳�ꂽID�����݂���΂��̓��e��Ԃ�
	if(record_.find(pID) == record_.end()){
		return NullRecord;
	}

	return *record_[pID];
}

CsvReader::Record& CsvReader::operator[](int row)
{
	// �ʂ��ԍ�����ID������o���Ă��̓��e��Ԃ�
	if((int)id_.size() > row && row > -1){
		return *record_[id_[row]];
	}

	return NullRecord;
}