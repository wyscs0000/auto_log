#include "writeToFile.h"
#include <BoostLog.h>
#include "Config.h"
#include <time.h>
#ifdef _WIN32
#include <Windows.h>

#define CHECK_DIR	CheckDir_win
#else
#include <direct.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#define CHECK_DIR	CheckDir_linux
#endif


logToFile*	logToFile::m_pLog = NULL;

logToFile::logToFile()
{
}
logToFile::~logToFile()
{
	releaseFileHandle();
}
void logToFile::init()
{
	time_t curTime = ::time(NULL);
	struct tm*  cur_tm = localtime(&curTime);
	char csFileName[256] = { '\0' };
	sprintf(csFileName, "%s%d-%d-%d_%s", g_Config.m_strLogPath.c_str(), cur_tm->tm_year + 1900, cur_tm->tm_mon + 1, cur_tm->tm_mday, g_Config.m_strLogName.c_str());
	m_csFullFileName = csFileName;
	createFile();
}
bool logToFile::createFile()
{
	pFile = NULL;
	if (!GetFileHandle())
	{
		std::string tmpFullFileName(m_csFullFileName.c_str());
		int nMark = tmpFullFileName.rfind("\\");
		if (nMark != std::string::npos)
		{
			std::string tmpDir(tmpFullFileName.substr(0, nMark));
			if (!CHECK_DIR(tmpDir))
				m_csFullFileName = g_Config.m_strLogName;
		}
	}
	if (GetFileHandle())
	{
		unsigned char header[3] = { 0xef, 0xbb, 0xbf };  // UTF-8 file header
		fwrite(header, sizeof(char), 3, GetFileHandle());
		return true;
	}

	printf("create log file error!\n");
	return false;
}
void	logToFile::writelog(const std::string& strlog)
{
	if (GetFileHandle())
	{
		if (0 >= fprintf_s(GetFileHandle(), "%s\n\r", To_utf8(strlog)))
		{
			LOG_INFO << "write log error!" << LOG_END;
			return;
		}
		//char ch[] = "\n\r";
		//fwrite(ch, 2, 1, GetFileHandle());
		fflush(GetFileHandle());
		////²âÊÔ
		//printf("%s\n", strlog.c_str());
	}
}
FILE* logToFile::GetFileHandle()
{
	if (!pFile)
	{
		pFile = fopen(m_csFullFileName.c_str(), "ab+");
	}
	return pFile;
}
void logToFile::releaseFileHandle()
{
	if (pFile)
		fclose(pFile);
}
const char*	 logToFile::To_utf8(const std::string& strLog)
{
#ifdef _WIN32
	wchar_t wc[MAX_LOG_LEN] = { '\0' };
	int nSize = MultiByteToWideChar(CP_ACP, 0, strLog.c_str(), strLog.length(), wc, MAX_LOG_LEN);
	if (nSize > 0)
	{
		memset(utf8, 0, MAX_LOG_LEN);
		nSize = WideCharToMultiByte(CP_UTF8, 0, wc, wcslen(wc), utf8, MAX_LOG_LEN, NULL, NULL);
		return utf8;
	}
	return NULL;
#else
	return strLog.c_str();
#endif
}
logToFile* logToFile::GetSignleton()
{
	if (NULL == m_pLog)
		m_pLog = new logToFile();
	return m_pLog;
}

bool CheckDir_linux(std::string& strPath)
{
#ifndef _WIN32
	size_t pre = 0, pos;
	std::string dir;
	int mdret;

	if (strPath[strPath.size() - 1] != '/'){
		// force trailing / so we can handle everything in loop  
		strPath += '/';
	}

	while ((pos = strPath.find_first_of('/', pre)) != std::string::npos){
		dir = strPath.substr(0, pos++);
		pre = pos;
		if (dir.size() == 0) continue; // if leading / first time is 0 length  
		if ((mdret = ::mkdir(dir.c_str())) && errno != EEXIST){
			return false;
		}
	}
#endif
	return true;
}
bool CheckDir_win(std::string& strPath)
{
#ifdef _WIN32
	if (!CreateDirectoryA(strPath.c_str(), NULL))
	{
		if (ERROR_PATH_NOT_FOUND == GetLastError())
		{
			int nMark = strPath.rfind("\\");
			if (nMark != std::string::npos)
			{
				std::string tmpDir(strPath.substr(0, nMark));
				if (CheckDir_win(tmpDir))
				{
					if (!CreateDirectoryA(strPath.c_str(), NULL))
					{
						printf("create Dir: %s	error\n", strPath.c_str());
						return false;
					}
				}
			}
			else
			{
				printf("create Dir: %s	error\n", strPath.c_str());
				return false;
			}

		}
		else
		{
			printf("create Dir: %s	error\n", strPath.c_str());
			return false;
		}
	}
#endif
	return true;
}
