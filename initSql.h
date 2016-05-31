#ifndef INITSQL_H_H
#define INITSQL_H_H

#include <winsock.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif
#include <string>
#include <vector>
#include <map>
#include "mysql.h"
#include "stdlib.h"
#ifdef _WIN32


typedef std::map<int,std::string> MapString;
typedef std::map<std::string, std::string> Value;
typedef std::vector<Value> valueList;

#define  _SPLIT_ ","

class TableFile
{
public:
	TableFile(const char* pFullPath);
	~TableFile();

	std::string m_strClassName; // ������

	MapString	m_arrName;		// �е����ƣ�ԭʼ���ƣ�
	MapString	m_arrComments;	// �е�ע��
	valueList	m_valueList;	//ÿһ�е�ֵ��Ϣ
};

void ParseLine2List(std::string& strLine, std::string& strSplit, MapString& arrValueList);

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

bool createDbTable(const char* db_ip, const int db_port, const char* db_name, const char* user_name, const char* pass,const char* tableName,char* sqlBuff,int nLen);

bool GetSqlBuff(Value& _field, const char* tableName,char* __out csBuff,int& nLen);

#endif