#ifndef _write_to_file_h_h
#define _write_to_file_h_h
#ifdef _WIN32
#include <string>
#else
#include <string.h>
#endif
#include <stdio.h> 
#define MAX_LOG_LEN	1024
class logToFile
{
public:
	logToFile();
	~logToFile();
public:
	void				init();
	void				writelog(const std::string& strlog);
	bool				createFile();
	static logToFile*	GetSignleton();
	FILE*				GetFileHandle();
	void				releaseFileHandle();
	const char*			To_utf8(const std::string& strLog);
private:
	static logToFile*	m_pLog;
	FILE*				pFile;
	std::string			m_csFullFileName;
	char				utf8[MAX_LOG_LEN];
};
#define g_logToFile logToFile::GetSignleton()

bool CheckDir_linux(std::string& strPath);
bool CheckDir_win(std::string& strPath);
#endif // !_write_to_file_h_h
