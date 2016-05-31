#ifndef __initTable_h__
#define __initTable_h__

#include <string>
#include <vector>
#include <map>

enum FieldType
{
	FT_int = 0,		//整数
	FT_float,		//浮点数
	FT_string,		//字符串
	FT_array,		//不定长整数数组
	FT_struct,		//子结构
	FT_unknow,
};

class ChildStruct
{
public:
	ChildStruct();
	~ChildStruct();

	std::vector<std::string> m_arrChildName;	// 子结构的字段名称
	std::vector<FieldType> m_arrFieldType;		// 子结构的字段类型
};

class TableFile
{
public:
	TableFile(const char* pFullPath);
	~TableFile();

	std::string m_strClassName; // 类名称

	std::vector<std::string> m_arrName;		// 列的名称（原始名称）
	std::vector<std::string> m_arrType;		// 列的类型标识
	std::vector<std::string> m_arrComments;	// 列的注释

	std::vector<bool> m_arrIsKey;			// 列是否是主键
	std::vector<bool> m_arrIsList;			// 列是否是不定长数组
	std::vector<FieldType> m_arrFieldType;	// 具体列类型
	std::map<int, ChildStruct> m_mapChildStruct;	// 嵌套的子结构字段名称，类型映射表
};

void ParseLine2List(std::string& strLine, std::string& strSplit, std::vector<std::string>& arrValueList);

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

#endif // __initTable_h__