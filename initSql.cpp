#include "initSql.h"

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

bool IsTextUTF8(char* str, int length)
{
	DWORD nBytes = 0;//UFT8����1-6���ֽڱ���,ASCII��һ���ֽ�
	BYTE chr;
	bool bAllAscii = true; //���ȫ������ASCII, ˵������UTF-8
	for (int i = 0; i < length; i++)
	{
		chr = *(str + i);
		if ((chr & 0x80) != 0) // �ж��Ƿ�ASCII����,�������,˵���п�����UTF-8,ASCII��7λ����,����һ���ֽڴ�,���λ���Ϊ0,o0xxxxxxx
		{
			bAllAscii = false;
		}

		if (nBytes == 0) //�������ASCII��,Ӧ���Ƕ��ֽڷ�,�����ֽ���
		{
			if (chr >= 0x80)
			{
				if (chr >= 0xFC && chr <= 0xFD)
					nBytes = 6;
				else if (chr >= 0xF8)
					nBytes = 5;
				else if (chr >= 0xF0)
					nBytes = 4;
				else if (chr >= 0xE0)
					nBytes = 3;
				else if (chr >= 0xC0)
					nBytes = 2;
				else
				{
					return false;
				}
				nBytes--;
			}
		}
		else //���ֽڷ��ķ����ֽ�,ӦΪ 10xxxxxx
		{
			if ((chr & 0xC0) != 0x80)
			{
				return false;
			}
			nBytes--;
		}
	}
	if (nBytes > 0) //Υ������
	{
		return false;
	}
	if (bAllAscii) //���ȫ������ASCII, ˵������UTF-8
	{
		return false;
	}
	return true;
}

std::string string_To_UTF8(const std::string & str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}

std::string UTF8_To_string(const std::string & str)
{
	int nwLen = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

	wchar_t * pwBuf = new wchar_t[nwLen + 1];//һ��Ҫ��1����Ȼ�����β��
	memset(pwBuf, 0, nwLen * 2 + 2);

	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char * pBuf = new char[nLen + 1];
	memset(pBuf, 0, nLen + 1);

	WideCharToMultiByte(CP_ACP, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr = pBuf;

	delete[]pBuf;
	delete[]pwBuf;

	pBuf = NULL;
	pwBuf = NULL;

	return retStr;
}

void ParseLine2List(std::string& strLine, std::string& strSplit, MapString& arrValueList)
{
	size_t npos = strLine.find(strSplit);
	size_t nlastpos = 0;
	int i = 0;
	while (npos != std::string::npos)
	{
		if (strLine.substr(nlastpos, npos - nlastpos).find("insert") != std::string::npos && strSplit == ",")
		{
			int nEndPos = strLine.find("\"", static_cast<unsigned int>(nlastpos + 5));
			if (nEndPos != std::string::npos)
			{
				npos = nEndPos + 1;
				/*arrValueList[i++] = strLine.substr(nlastpos, nEndPos - nlastpos);
				nlastpos = nEndPos + 2;*/
				/*continue;*/
			}
			else
				break;
		}
		arrValueList[i++] = strLine.substr(nlastpos, npos - nlastpos);
		/*arrValueList.push_back(strLine.substr(nlastpos, npos - nlastpos));*/
		nlastpos = npos + 1;
		npos = strLine.find(strSplit, static_cast<unsigned int>(nlastpos));
	}
	if (nlastpos != strLine.length())
	{
		arrValueList[i++] = strLine.substr(nlastpos, strLine.length() - nlastpos);
		/*arrValueList.push_back(strLine.substr(nlastpos, strLine.length() - nlastpos));*/
	}
}

wchar_t* CharToWchar(wchar_t* wscStr, char* strStr)
{
	int len = MultiByteToWideChar(CP_ACP, 0, strStr, strlen(strStr), NULL, 0);
	MultiByteToWideChar(CP_ACP, 0, strStr, strlen(strStr), wscStr, len);
	wscStr[len] = '\0';
	return wscStr;
}

char* WcharToChar(char* strStr, wchar_t* wscStr)
{
	int len = WideCharToMultiByte(CP_ACP, 0, wscStr, wcslen(wscStr), NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, wscStr, wcslen(wscStr), strStr, len, NULL, NULL);
	strStr[len] = '\0';
	return strStr;
}

int GetLine(char *pLine, FILE *pTableFile)
{
	if (NULL != pLine && NULL != pTableFile)
	{
		int nLineIndex = 0;
		char cLineChar = -1;
		while (true)
		{
			cLineChar = fgetc(pTableFile);
			if ('\n' != cLineChar && EOF != cLineChar)
			{
				pLine[nLineIndex] = cLineChar;
				nLineIndex++;
			}
			else
			{
				pLine[nLineIndex] = 0;
				nLineIndex++;
				break;
			}
		}

		if (nLineIndex >= 3)
		{
			if ((BYTE)pLine[0] == 0xef && (BYTE)pLine[1] == 0xbb && (BYTE)pLine[2] == 0xbf) //UTF8 BOM ͷ
			{
				memcpy(pLine, pLine + 3, nLineIndex - 3);
				nLineIndex = nLineIndex - 3;
			}
		}

		return nLineIndex;
	}
	else
	{
		return 0;
	}
}

std::string& UpCaseFirstChar(std::string& str)
{
	if ('a' <= str[0])
	{
		str[0] = str[0] - ('a' - 'A');
	}
	return str;
}

bool IsWithNum(const std::string& str)
{
	for (int i = 0; i < strlen(str.c_str()); i++)
	{
		if ('0' <= str[i] && str[i] <= '9')
		{
			return true;
		}
	}

	return false;
}

std::string& SubstrWithoutNum(std::string& str)
{
	int nCurrLen = strlen(str.c_str());
	int i = 0;
	while (i < nCurrLen)
	{
		if ('0' <= str[i] && str[i] <= '9')
		{
			// ����ַ�ǰ��
			int j = i;
			while (j < nCurrLen)
			{
				if (j < nCurrLen - 1)
				{
					str[j] = str[j + 1];
					j++;
				}
				else
				{
					str[j] = 0;
					break;
				}
			}
			nCurrLen--;
		}
		else
		{
			i++;
		}
	}

	return str;
}

int StrcmpWithoutNum(const std::string& str1, const std::string& str2)
{
	std::string strTemp1 = str1;
	std::string strTemp2 = str2;
	return strcmp(SubstrWithoutNum(strTemp1).c_str(), SubstrWithoutNum(strTemp2).c_str());
}

int IsRepeatStr(const std::string& str, const std::vector<std::string>& arrStrList)
{
	int nRepeatCount = 0;
	for (int i = 0; i < arrStrList.size(); i++)
	{
		if (0 == strcmp(arrStrList[i].c_str(), str.c_str()))
		{
			nRepeatCount++;
		}
	}
	return nRepeatCount;
}

int IsRepeatStrInFront(int nCurrIndex, const std::vector<std::string>& arrStrList)
{
	int nRepeatCount = 0;
	for (int i = 0; i < nCurrIndex; i++)
	{
		if (0 == strcmp(arrStrList[i].c_str(), arrStrList[nCurrIndex].c_str()))
		{
			nRepeatCount++;
		}
	}
	return nRepeatCount;
}

TableFile::TableFile(const char* pFullPath)
{
	m_arrName.clear();
	m_arrComments.clear();

	m_valueList.clear();

	//��ȡ�ļ�·������ȡ������
	std::string strFullPath = pFullPath;
	std::string strFileName = strFullPath.substr(strFullPath.find_last_of("/") + 1);
	m_strClassName = strFileName.substr(0, strFileName.find("."));
	UpCaseFirstChar(m_strClassName);

	//����ͷ
	FILE *pTableFile;
	int nError = fopen_s(&pTableFile, pFullPath, "r");
	if (NULL != pTableFile)
	{
		char *pStrLine = new char[1024 * 10];
		int nLen = 0;
		if (NULL != pStrLine)
		{
			std::string strLine = pStrLine;
			std::string strSplit = ",";

			//������
			memset(pStrLine, 0x00, sizeof(char)* 1024 * 10);
			nLen = GetLine(pStrLine, pTableFile);
			strLine = pStrLine;
			ParseLine2List(strLine, strSplit, m_arrName);

			//��ע��
			memset(pStrLine, 0x00, sizeof(char)* 1024 * 10);
			nLen = GetLine(pStrLine, pTableFile);
			if (IsTextUTF8(pStrLine, nLen))
			{
				std::string temp = pStrLine;
				strLine = UTF8_To_string(temp);
				printf("\n%s is utf8\n", m_strClassName.c_str());
			}
			else
			{
				strLine = pStrLine;
				printf("\n%s not utf8\n", m_strClassName.c_str());
			}
			ParseLine2List(strLine, strSplit, m_arrComments);

			//������������
			while (true)
			{
				memset(pStrLine, 0x00, sizeof(char)* 1024 * 10);
				nLen = GetLine(pStrLine, pTableFile);
				if (pStrLine[0] == '\0')
					break;
				strLine = pStrLine;
				MapString tmpValue;
				Value tmpOneLine;
				ParseLine2List(strLine, strSplit, tmpValue);
				MapString::iterator itor = tmpValue.begin();
				for (; itor != tmpValue.end(); itor++)
				{
					MapString::iterator it = m_arrName.find(itor->first);
					if (it != m_arrName.end())
					{
						tmpOneLine[it->second] = itor->second;
					}
				}
				m_valueList.push_back(tmpOneLine);
			}

			delete[] pStrLine;
		}

		fclose(pTableFile);
	}
}

TableFile::~TableFile()
{
	m_arrName.clear();
	m_arrComments.clear();
	m_valueList.clear();
}
bool createDbTable(const char* db_ip, const int db_port, const char* db_name,\
	const char* user_name, const char* pass, const char* tableName, char* sqlBuff, int nLen)
{
	MYSQL		*mysql = NULL;
	MYSQL_RES *res = NULL;
	MYSQL_ROW row;
	mysql = mysql_init(NULL);
	if (NULL == mysql)
	{
		printf("mysql init error\n");
		return false;
	}
	//���� utf8
	if (mysql_options(mysql, MYSQL_SET_CHARSET_NAME, "utf8"))
	{
		printf(mysql_error(mysql));
		return false;
	}
	//����DB
	if (!mysql_real_connect(mysql, db_ip, user_name, pass, db_name, db_port, NULL, 0)){
		printf(mysql_error(mysql));
		return false;
	}
	//�Ȳ�ѯ���Ƿ����
	char csSql[512] = {'\0'};
	sprintf_s(csSql, 512, "select count(*) from information_schema.tables where table_name = '%s'", tableName);
	int ret = mysql_real_query(mysql, csSql, strlen(csSql));
	if (ret){
		mysql_close(mysql);
		return false;
	}
	res = mysql_store_result(mysql);
	if (res == NULL){
		mysql_free_result(res);
		mysql_close(mysql);
		return false;
	}
	row = mysql_fetch_row(res);
	unsigned long* row_len = mysql_fetch_lengths(res);
	if (row_len == NULL){
		mysql_free_result(res);
		mysql_close(mysql);
		return false;
	}
	char nCount[8] = {'\0'};
	memcpy(nCount, row[0], *row_len);
	if (atoi(nCount) != 0)
	{
		printf("table: %s already created!\n", tableName);
		mysql_close(mysql);
		mysql_free_result(res);
		return false;
	}
	mysql_free_result(res);
	//������
	ret = mysql_real_query(mysql, sqlBuff, nLen);
	if (ret){
		printf(mysql_error(mysql));
		mysql_close(mysql);
		return false;
	}
	mysql_close(mysql);
	return true;
}
bool GetSqlBuff(Value& _field, const char* tableName, char* __out csBuff, int& nLen)
{
	std::string strSql;
	strSql += "create table ";
	strSql += tableName;
	strSql += "(nid int not null PRIMARY KEY  AUTO_INCREMENT,";
	int nCount = 1;
	for (Value::iterator itor = _field.begin(); itor != _field.end(); itor++, nCount++)
	{
		strSql += itor->first;
		strSql += " ";
		if (itor->second.find("int") != std::string::npos || itor->second.find("unsigned") != std::string::npos)
		{
			strSql += "int";
		}
		else if (itor->second.find("double") != std::string::npos || itor->second.find("float") != std::string::npos)
		{
			strSql += "float";
		}
		else if (itor->second.find("char*") != std::string::npos)
		{
			if (itor->first.find("name") != std::string::npos)
				strSql += "varchar(40)";
			else if (itor->first.find("time") != std::string::npos || itor->first.find("day") != std::string::npos)
				strSql += "varchar(64)";
			else
				strSql += "varchar(1024)";		
		}
		else
			strSql += "int";

		if (nCount < _field.size())
			strSql += ",";
	}
	strSql += ")ENGINE = innodb  DEFAULT CHARSET=utf8";
	
	memcpy(csBuff, strSql.c_str(), strSql.length());
	nLen = strSql.length();
	return true;
}