#ifndef __initTable_h__
#define __initTable_h__

#include <string>
#include <vector>
#include <map>

enum FieldType
{
	FT_int = 0,		//����
	FT_float,		//������
	FT_string,		//�ַ���
	FT_array,		//��������������
	FT_struct,		//�ӽṹ
	FT_unknow,
};

class ChildStruct
{
public:
	ChildStruct();
	~ChildStruct();

	std::vector<std::string> m_arrChildName;	// �ӽṹ���ֶ�����
	std::vector<FieldType> m_arrFieldType;		// �ӽṹ���ֶ�����
};

class TableFile
{
public:
	TableFile(const char* pFullPath);
	~TableFile();

	std::string m_strClassName; // ������

	std::vector<std::string> m_arrName;		// �е����ƣ�ԭʼ���ƣ�
	std::vector<std::string> m_arrType;		// �е����ͱ�ʶ
	std::vector<std::string> m_arrComments;	// �е�ע��

	std::vector<bool> m_arrIsKey;			// ���Ƿ�������
	std::vector<bool> m_arrIsList;			// ���Ƿ��ǲ���������
	std::vector<FieldType> m_arrFieldType;	// ����������
	std::map<int, ChildStruct> m_mapChildStruct;	// Ƕ�׵��ӽṹ�ֶ����ƣ�����ӳ���
};

void ParseLine2List(std::string& strLine, std::string& strSplit, std::vector<std::string>& arrValueList);

// �����ֽڻ�ת
wchar_t* CharToWchar(wchar_t* wscStr, char* strStr);
char* WcharToChar(char* strStr, wchar_t* wscStr);

// ���ļ���һ��
int GetLine(char *pLine, FILE *pTableFile);

// �ַ������ַ���д
std::string& UpCaseFirstChar(std::string& str);

// �ж�һ���ַ����Ƿ��������
bool IsWithNum(const std::string& str);

// �ض��ַ���ĩβ������
std::string& SubstrWithoutNum(std::string& str);

// �ж������ַ����ų��������⣬�Ƿ����
int StrcmpWithoutNum(std::string& str1, std::string& str2);

// �ж�һ���ַ����Ƿ��ظ����֣����س��ֵĴ���
int IsRepeatStr(const std::string& str, const std::vector<std::string>& arrStrList);

// �ж�һ���ַ��������е�ǰ�±�nCurrIndex���ַ����Ƿ�����֮ǰ���ַ����ظ�
int IsRepeatStrInFront(int nCurrIndex, const std::vector<std::string>& arrStrList);

#endif // __initTable_h__