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

	std::string m_strClassName; // 类名称

	MapString	m_arrName;		// 列的名称（原始名称）
	MapString	m_arrComments;	// 列的注释
	valueList	m_valueList;	//每一行的值信息
};

void ParseLine2List(std::string& strLine, std::string& strSplit, MapString& arrValueList);

// 单宽字节互转
wchar_t* CharToWchar(wchar_t* wscStr, char* strStr);
char* WcharToChar(char* strStr, wchar_t* wscStr);

// 读文件的一行
int GetLine(char *pLine, FILE *pTableFile);

// 字符串首字符大写
std::string& UpCaseFirstChar(std::string& str);

// 判断一个字符串是否包含数字
bool IsWithNum(const std::string& str);

// 截断字符串末尾的数字
std::string& SubstrWithoutNum(std::string& str);

// 判断两个字符串排除数字以外，是否相等
int StrcmpWithoutNum(std::string& str1, std::string& str2);

// 判断一个字符串是否重复出现，返回出现的次数
int IsRepeatStr(const std::string& str, const std::vector<std::string>& arrStrList);

// 判断一个字符串数组中当前下标nCurrIndex的字符串是否与其之前的字符串重复
int IsRepeatStrInFront(int nCurrIndex, const std::vector<std::string>& arrStrList);

bool createDbTable(const char* db_ip, const int db_port, const char* db_name, const char* user_name, const char* pass,const char* tableName,char* sqlBuff,int nLen);

bool GetSqlBuff(Value& _field, const char* tableName,char* __out csBuff,int& nLen);

#endif