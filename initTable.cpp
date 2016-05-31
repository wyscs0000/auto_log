#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>

#include "initTable.h"

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

void ParseLine2List(std::string& strLine, std::string& strSplit, std::vector<std::string>& arrValueList)
{
	size_t npos = strLine.find(strSplit);
	size_t nlastpos = 0;
	while (npos != std::string::npos)
	{
		arrValueList.push_back(strLine.substr(nlastpos, npos - nlastpos));
		nlastpos = npos + 1;
		npos = strLine.find(strSplit, static_cast<unsigned int>(nlastpos));
	}
	if (nlastpos != strLine.length())
	{
		arrValueList.push_back(strLine.substr(nlastpos, strLine.length() - nlastpos));
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

ChildStruct::ChildStruct()
{
	m_arrChildName.clear();
	m_arrFieldType.clear();
}

ChildStruct::~ChildStruct()
{
	m_arrChildName.clear();
	m_arrFieldType.clear();
}

TableFile::TableFile(const char* pFullPath)
{
	m_arrName.clear();
	m_arrType.clear();
	m_arrComments.clear();

	m_arrIsKey.clear();
	m_arrIsList.clear();
	m_arrFieldType.clear();
	m_mapChildStruct.clear();

	//��ȡ�ļ�·������ȡ������
	std::string strFullPath = pFullPath;
	std::string strFileName = strFullPath.substr(strFullPath.find_last_of("/") + 1);
	m_strClassName = strFileName.substr(0, strFileName.find("."));
	UpCaseFirstChar(m_strClassName);

	//����ͷ
	FILE *pTableFile;
	fopen_s(&pTableFile, pFullPath, "r");
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

			//������
			memset(pStrLine, 0x00, sizeof(char)* 1024 * 10);
			nLen = GetLine(pStrLine, pTableFile);
			strLine = pStrLine;
			ParseLine2List(strLine, strSplit, m_arrType);
			for (size_t i = 0; i < m_arrType.size(); i++)
			{
				std::string strKeyType = m_arrType[i].substr(0, m_arrType[i].find(":"));
				//�ж��Ƿ�������
				if (0 == strcmp("k", strKeyType.c_str()))
				{
					m_arrIsKey.push_back(true);
				}
				else
				{
					m_arrIsKey.push_back(false);
				}
				//�ж��Ƿ��ǲ���������
				if (0 == strcmp("lst", strKeyType.c_str()))
				{
					m_arrIsList.push_back(true);
				}
				else
				{
					m_arrIsList.push_back(false);
				}

				//����/�����������ǰ׺��ʶȥ��
				m_arrType[i] = m_arrType[i].substr(m_arrType[i].find(":") + 1, std::string::npos);

				std::vector<std::string> arrTypeList;
				std::string arrTypeSplit = ";";
				ParseLine2List(m_arrType[i], arrTypeSplit, arrTypeList);
				if (0 == arrTypeList.size())
				{
					m_arrFieldType.push_back(FT_unknow);
				}
				else if (1 == arrTypeList.size())
				{
					//���ֶ����ͣ��жϾ����ֶ�����
					if (0 == strcmp("i", arrTypeList[0].c_str()))
					{
						m_arrFieldType.push_back(FT_int);
					}
					else if (0 == strcmp("f", arrTypeList[0].c_str()))
					{
						m_arrFieldType.push_back(FT_float);
					}
					else if (0 == strcmp("s", arrTypeList[0].c_str()))
					{
						m_arrFieldType.push_back(FT_string);
					}
					else if (0 == strcmp("a", arrTypeList[0].c_str()))
					{
						m_arrFieldType.push_back(FT_array);
					}
					else
					{
						m_arrFieldType.push_back(FT_unknow);
					}
				}
				else // 1 < arrTypeList.size()
				{
					//���ֶ�����
					m_arrFieldType.push_back(FT_struct);

					//�����ӽṹ
					ChildStruct child_struct;
					for (size_t j = 0; j < arrTypeList.size(); j = j + 2)
					{
						if (0 == strcmp("i", arrTypeList[j + 1].c_str()))
						{
							child_struct.m_arrChildName.push_back(arrTypeList[j].c_str());
							child_struct.m_arrFieldType.push_back(FT_int);
						}
						else if (0 == strcmp("f", arrTypeList[j + 1].c_str()))
						{
							child_struct.m_arrChildName.push_back(arrTypeList[j].c_str());
							child_struct.m_arrFieldType.push_back(FT_float);
						}
						else if (0 == strcmp("s", arrTypeList[j + 1].c_str()))
						{
							child_struct.m_arrChildName.push_back(arrTypeList[j].c_str());
							child_struct.m_arrFieldType.push_back(FT_string);
						}
						else if (0 == strcmp("a", arrTypeList[j + 1].c_str()))
						{
							child_struct.m_arrChildName.push_back(arrTypeList[j].c_str());
							child_struct.m_arrFieldType.push_back(FT_array);
						}
						else
						{
							child_struct.m_arrChildName.push_back(arrTypeList[j].c_str());
							child_struct.m_arrFieldType.push_back(FT_unknow);
						}
					}
					m_mapChildStruct[i] = child_struct;
				}
			}

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

			delete[] pStrLine;
		}

		fclose(pTableFile);
	}
}

TableFile::~TableFile()
{
	m_arrName.clear();
	m_arrType.clear();
	m_arrComments.clear();

	m_arrIsKey.clear();
	m_arrIsList.clear();
	m_arrFieldType.clear();
	m_mapChildStruct.clear();
}