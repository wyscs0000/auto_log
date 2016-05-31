// ParseTable.cpp : 定义控制台应用程序的入口点。
//

#include <windows.h>
#include <tchar.h>

#include <vector>

#include "initSql.h"

void ParseTable2H(const char* outFilePath, const TableFile& inTableFile)
{
	FILE *pHFile;
	std::string strFullPach = outFilePath;
	strFullPach = strFullPach + inTableFile.m_strClassName.c_str();
	strFullPach = strFullPach + ".h";
	int nErroCode = fopen_s(&pHFile, strFullPach.c_str(), "w");
	int nCode = GetLastError();
	printf("Parse %s\n", strFullPach.c_str());
	if (NULL != pHFile)
	{
		fprintf_s(pHFile, "#ifndef __%s_h__\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "#define __%s_h__\n\n", inTableFile.m_strClassName.c_str());

		fprintf_s(pHFile, "#include <map>\n");
		fprintf_s(pHFile, "#include <string>\n");
		fprintf_s(pHFile, "#include <list>\n");
		fprintf_s(pHFile, "#include <windows.h>\n\n");
		
		//class 子结构 {...};
		fprintf_s(pHFile, "class %s\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "public:\n");
		//构造析构
		fprintf_s(pHFile, "\t\t%s();\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "\t\t~%s();\n\n", inTableFile.m_strClassName.c_str());
		for (int i = 0; i < inTableFile.m_valueList.size(); i++)
		{
			//XXX() {...}
			Value tmpValueList = inTableFile.m_valueList[i];
			Value::iterator itor_name = tmpValueList.find("LogTableName");
			if (itor_name != tmpValueList.end())
			{
				Value::iterator itor_sql = tmpValueList.find("sqlInfo");
				if (itor_sql != tmpValueList.end())
				{
					fprintf_s(pHFile, "\t\tvoid ");
					fprintf_s(pHFile, "%s (", itor_name->second.c_str());
					std::string sqlinfo = itor_sql->second;

					// insert into XXXTableName(xxx,xxx,xxx) 括号中字段解析
					std::string _split = _SPLIT_;
					MapString fieldList;	//字段名称
					MapString field_type;	//字段类型（值类型）
					size_t npos_begin = sqlinfo.find("(");
					size_t npos_end = sqlinfo.find(")");
					if (npos_begin != std::string::npos && npos_end != std::string::npos)
					{
						ParseLine2List(sqlinfo.substr(npos_begin+1, npos_end - npos_begin-1), _split, fieldList);
					}
					else
					{
						printf("表：%s 解析字段时 数据错误！", itor_name->second.c_str());
						continue;
					}
					std::string value_string = sqlinfo.substr(npos_end + 1, sqlinfo.length() - npos_end);
					npos_begin = value_string.find("(");
					npos_end = value_string.find(")");
					if (npos_begin != std::string::npos && npos_end != std::string::npos)
					{
						ParseLine2List(value_string.substr(npos_begin + 1, npos_end - npos_begin-1), _split, field_type);
					}
					else
					{
						printf("表：%s 解析值类型时 数据错误！", itor_name->second.c_str());
						continue;
					}
					for (MapString::iterator _it = field_type.begin(); _it != field_type.end();)
					{
						std::string tmpType = _it->second;
						size_t npos_begin = tmpType.find("%");
						if (npos_begin != std::string::npos && 1 <= tmpType.length() - npos_begin)
						{
							if (tmpType[npos_begin + 1] == 'd' || tmpType[npos_begin + 1] == 'i')
							{
								_it->second = "int";
							}
							else if (tmpType[npos_begin + 1] == 'f')
							{
								_it->second = "double";
							}
							else if (tmpType[npos_begin + 1] == 'u')
							{
								_it->second = "unsigned int";
							}
							else if (tmpType[npos_begin + 1] == 's')
							{
								_it->second = "const char*";
							}
							else
							{
								field_type.erase(_it++);
								printf("表：%s 解析值类型时 出现未知类型：%s", itor_name->second.c_str(), tmpType.c_str());
								continue;
							}
							_it++;
						}
						else
							field_type.erase(_it++);
					}
					if (field_type.size() > fieldList.size())
					{
						printf("表：%s 解析值类型时 数据错误！", itor_name->second.c_str());
						continue;
					}

					{
						//这里可以创建mysql表
						int nIsCreateTable = 0;
						Value::iterator itor_tmp = tmpValueList.find("isCreateTable");
						if (itor_tmp != tmpValueList.end())
						{
							nIsCreateTable = atoi(itor_tmp->second.c_str());
							if (nIsCreateTable)
							{
								Value create_table_field;
								for (MapString::iterator itor_ = fieldList.begin(); itor_ != fieldList.end(); itor_++)
								{
									MapString::iterator _it_field_type = field_type.find(itor_->first);
									if (_it_field_type != field_type.end())
									{
										create_table_field[itor_->second] = _it_field_type->second;
									}
									else
										create_table_field[itor_->second] = " ";
								}
								char sql[512] = { '\0' };
								int nSqlLen = 0;
								if (GetSqlBuff(create_table_field, itor_name->second.c_str(), sql, nSqlLen))
								{
									std::string db_ip, db_port, db_name, user, pass;
									itor_tmp = tmpValueList.find("db_ip");
									if (itor_tmp != tmpValueList.end())
										db_ip = itor_tmp->second;
									itor_tmp = tmpValueList.find("db_port");
									if (itor_tmp != tmpValueList.end())
										db_port = itor_tmp->second;
									itor_tmp = tmpValueList.find("db_name");
									if (itor_tmp != tmpValueList.end())
										db_name = itor_tmp->second;
									itor_tmp = tmpValueList.find("name");
									if (itor_tmp != tmpValueList.end())
										user = itor_tmp->second;
									itor_tmp = tmpValueList.find("pass");
									if (itor_tmp != tmpValueList.end())
										pass = itor_tmp->second;
									createDbTable(db_ip.c_str(), atoi(db_port.c_str()), db_name.c_str(), user.c_str(), pass.c_str(), itor_name->second.c_str(), sql, nSqlLen);
								}

							}
						}
						
					
					}
					int x_index = 1;
					for (MapString::iterator _it = field_type.begin(); _it != field_type.end(); _it++, x_index++)
					{
						fprintf_s(pHFile, "%s ", _it->second.c_str());
						MapString::iterator _it_field = fieldList.find(_it->first);
						if (_it_field != fieldList.end())
						{
							fprintf_s(pHFile, "%s", _it_field->second.c_str());
						}
						if (x_index < field_type.size())
						{
							fprintf_s(pHFile, ", ");
						}
					}
					fprintf_s(pHFile, ");\n");
				}
			}
		}
		fprintf_s(pHFile, "public:\n");

		//fprintf_s(pHFile, "\t\tbool PopOneLog(char* csLogBuf,int& nLen);\n");
		fprintf_s(pHFile, "\t\tbool PopOneLog(std::string& strLog);\n");
		fprintf_s(pHFile, "\t\tvoid PushOneLog(std::string& strLog);\n");

		fprintf_s(pHFile, "\t\tstatic %s* GetSingleton();\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "private:\n");
		fprintf_s(pHFile, "\t\tstd::list<std::string>  m_logList;\n");
		fprintf_s(pHFile, "\t\tchar					 m_tmpBuf[1024*4];\n");
		fprintf_s(pHFile, "\t\tstatic %s*			 m_singleton;\n", inTableFile.m_strClassName.c_str());

		fprintf_s(pHFile, "\n};\n");
		fprintf_s(pHFile, "#define g_%s %s::GetSingleton()\n\n", inTableFile.m_strClassName.c_str(), inTableFile.m_strClassName.c_str());

		fprintf_s(pHFile, "class lock_multiThread\n{\n");
		fprintf_s(pHFile, "public:\n");
		fprintf_s(pHFile, "\t\tlock_multiThread();\n");
		fprintf_s(pHFile, "\t\t~lock_multiThread();\n");
		fprintf_s(pHFile, "\t\tvoid lock();\n");
		fprintf_s(pHFile, "\t\tvoid unlock();\n");
		fprintf_s(pHFile, "private:\n");
		fprintf_s(pHFile, "\t\tCRITICAL_SECTION cs;\n");
		fprintf_s(pHFile, "};\n");
		fprintf_s(pHFile, "\n");
		fprintf_s(pHFile, "#endif\n");
		fclose(pHFile);
	}
}


void ParseTable2CPP(const char* outFilePath, const TableFile& inTableFile)
{
	FILE *pHFile;
	std::string strFullPach = outFilePath;
	strFullPach = strFullPach + inTableFile.m_strClassName.c_str();
	strFullPach = strFullPach + ".cpp";
	fopen_s(&pHFile, strFullPach.c_str(), "w");
	printf("Parse %s\n", strFullPach.c_str());
	if (NULL != pHFile)
	{
		fprintf_s(pHFile, "#include \"%s.h\"\n\n", inTableFile.m_strClassName.c_str());
		//构造析构
		fprintf_s(pHFile, "%s* %s::m_singleton = NULL;\n", inTableFile.m_strClassName.c_str(), inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "%s::%s()\n", inTableFile.m_strClassName.c_str(), inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "}\n");
		fprintf_s(pHFile, "%s::~%s()\n", inTableFile.m_strClassName.c_str(), inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "}\n");

		for (int i = 0; i < inTableFile.m_valueList.size(); i++)
		{
			//XXX() {...}
			Value tmpValueList = inTableFile.m_valueList[i];
			Value::iterator itor_name = tmpValueList.find("LogTableName");
			if (itor_name != tmpValueList.end())
			{
				Value::iterator itor_sql = tmpValueList.find("sqlInfo");
				if (itor_sql != tmpValueList.end())
				{
					fprintf_s(pHFile, "void ");
					fprintf_s(pHFile, "%s::%s (", inTableFile.m_strClassName.c_str(), itor_name->second.c_str());
					std::string sqlinfo = itor_sql->second;

					// insert into XXXTableName(xxx,xxx,xxx) 括号中字段解析
					std::string _split = _SPLIT_;
					MapString fieldList;	//字段名称
					MapString field_type;	//字段类型（值类型）
					size_t npos_begin = sqlinfo.find("(");
					size_t npos_end = sqlinfo.find(")");
					if (npos_begin != std::string::npos && npos_end != std::string::npos)
					{
						ParseLine2List(sqlinfo.substr(npos_begin+1, npos_end - npos_begin-1), _split, fieldList);
					}
					else
					{
						printf("表：%s 解析字段时 数据错误！", itor_name->second.c_str());
						continue;
					}
					std::string value_string = sqlinfo.substr(npos_end + 1, sqlinfo.length() - npos_end);
					npos_begin = value_string.find("(");
					npos_end = value_string.find(")");
					if (npos_begin != std::string::npos && npos_end != std::string::npos)
					{
						ParseLine2List(value_string.substr(npos_begin+1, npos_end - npos_begin-1), _split, field_type);
					}
					else
					{
						printf("表：%s 解析值类型时 数据错误！", itor_name->second.c_str());
						continue;
					}

					{
						//这里可以创建mysql表
					}
					for (MapString::iterator _it = field_type.begin(); _it != field_type.end();)
					{
						std::string tmpType = _it->second;
						size_t npos_begin = tmpType.find("%");
						if (npos_begin != std::string::npos && 1 <= tmpType.length() - npos_begin)
						{
							if (tmpType[npos_begin + 1] == 'd' || tmpType[npos_begin + 1] == 'i')
							{
								_it->second = "int";
							}
							else if (tmpType[npos_begin + 1] == 'f')
							{
								_it->second = "double";
							}
							else if (tmpType[npos_begin + 1] == 'u')
							{
								_it->second = "unsigned int";
							}
							else if (tmpType[npos_begin + 1] == 's')
							{
								_it->second = "const char*";
							}
							else
							{
								field_type.erase(_it++);
								printf("表：%s 解析值类型时 出现未知类型：%s", itor_name->second.c_str(), tmpType.c_str());
								continue;
							}
							_it++;
						}
						else
							field_type.erase(_it++);
					}
					if (field_type.size() > fieldList.size())
					{
						printf("表：%s 解析值类型时 数据错误！", itor_name->second.c_str());
						continue;
					}
					std::string tmp_type_format;
					std::string check_Null;
					int x_index = 1;
					for (MapString::iterator _it = field_type.begin(); _it != field_type.end(); _it++, x_index++)
					{
						fprintf_s(pHFile, "%s ", _it->second.c_str());
						MapString::iterator _it_field = fieldList.find(_it->first);
						if (_it_field != fieldList.end())
						{
							fprintf_s(pHFile, "%s", _it_field->second.c_str());
							tmp_type_format += _it_field->second;
							if (_it->second.find("*") != std::string::npos)
							{
								check_Null += "\tif(";
								check_Null += _it_field->second;
								check_Null += " == NULL)\n\t\treturn;\n";
							}
						}
						if (x_index < field_type.size())
						{
							tmp_type_format += ",";
							fprintf_s(pHFile, ", ");
						}
					}
					fprintf_s(pHFile, ")\n");
					fprintf_s(pHFile, "{\n");
					fprintf_s(pHFile, "%s", check_Null.c_str());
					fprintf_s(pHFile, "\tmemset(m_tmpBuf,0,sizeof(m_tmpBuf));\n");

					//防止sql语句结尾出现 空格或者tab 不容易查找
					size_t s_end = sqlinfo.rfind(")");
					if (sqlinfo[s_end+1] != '\"')
					{
						sqlinfo = sqlinfo.substr(0, s_end + 1);
						sqlinfo += "\"";
					}
					fprintf_s(pHFile, "\tsprintf_s(m_tmpBuf,sizeof(m_tmpBuf),%s,%s);\n", sqlinfo.c_str(), tmp_type_format.c_str());
					fprintf_s(pHFile, "\tstd::string strLog(m_tmpBuf);\n");

					fprintf_s(pHFile, "\tPushOneLog(strLog);\n"); 
					
					fprintf_s(pHFile, "}\n");
				}
			}
		}
		/*fprintf_s(pHFile, "bool %s::PopOneLog(char* csLogBuf,int& nLen)\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\tif(m_logList.size() != 0)\n");
		fprintf_s(pHFile, "\t{\n");
		fprintf_s(pHFile, "\t\tlock_multiThread m_lock;\n");
		fprintf_s(pHFile, "\t\tm_lock.lock();\n");
		fprintf_s(pHFile, "\t\tstd::vector<std::string>::iterator itor = m_logList.begin();\n");
		fprintf_s(pHFile, "\t\tmemcpy(csLogBuf,(*itor).c_str(),(*itor).length());\n");
		fprintf_s(pHFile, "\t\tnLen = (*itor).length();\n");
		fprintf_s(pHFile, "\t\tm_logList.erase(itor);\n");
		fprintf_s(pHFile, "\t\tm_lock.unlock();\n");
		fprintf_s(pHFile, "\t\treturn true;\n");
		fprintf_s(pHFile, "\t}\n");
		fprintf_s(pHFile, "\treturn false;\n");
		fprintf_s(pHFile, "}\n");*/

		fprintf_s(pHFile, "bool %s::PopOneLog(std::string& strLog)\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\tif(m_logList.size() != 0)\n");
		fprintf_s(pHFile, "\t{\n");
		fprintf_s(pHFile, "\t\tlock_multiThread m_lock;\n");
		fprintf_s(pHFile, "\t\tm_lock.lock();\n");
		fprintf_s(pHFile, "\t\tstd::list<std::string>::iterator itor = m_logList.begin();\n");
		fprintf_s(pHFile, "\t\tstrLog = *itor;\n");
		fprintf_s(pHFile, "\t\tm_logList.erase(itor);\n");
		fprintf_s(pHFile, "\t\tm_lock.unlock();\n");
		fprintf_s(pHFile, "\t\treturn true;\n");
		fprintf_s(pHFile, "\t}\n");
		fprintf_s(pHFile, "\treturn false;\n");
		fprintf_s(pHFile, "}\n");

		fprintf_s(pHFile, "void %s::PushOneLog(std::string& strLog)\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\tif(strLog.empty())\n");
		fprintf_s(pHFile, "\t\treturn;\n");
		fprintf_s(pHFile, "\tlock_multiThread m_lock;\n");
		fprintf_s(pHFile, "\tm_lock.lock();\n");
		fprintf_s(pHFile, "\tm_logList.push_back(strLog);\n");
		fprintf_s(pHFile, "\tm_lock.unlock();\n");
		fprintf_s(pHFile, "}\n");

		fprintf_s(pHFile, "%s* %s::GetSingleton()\n", inTableFile.m_strClassName.c_str(), inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\tlock_multiThread m_lock;\n");
		fprintf_s(pHFile, "\tm_lock.lock();\n");
		fprintf_s(pHFile, "\tif(m_singleton == NULL)\n");
		fprintf_s(pHFile, "\t{\n");
		fprintf_s(pHFile, "\t\tm_singleton = new %s;\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "\t}\n");
		fprintf_s(pHFile, "\tm_lock.unlock();\n");
		fprintf_s(pHFile, "\treturn m_singleton;\n", inTableFile.m_strClassName.c_str());
		fprintf_s(pHFile, "}\n\n");

		fprintf_s(pHFile, "lock_multiThread::lock_multiThread()\n");
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\t::InitializeCriticalSection(&cs);\n");
		fprintf_s(pHFile, "}\n");

		fprintf_s(pHFile, "lock_multiThread::~lock_multiThread()\n");
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\t::DeleteCriticalSection(&cs);\n");
		fprintf_s(pHFile, "}\n");

		fprintf_s(pHFile, "void lock_multiThread::lock()\n");
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\t::EnterCriticalSection(&cs);\n");
		fprintf_s(pHFile, "}\n");

		fprintf_s(pHFile, "void lock_multiThread::unlock()\n");
		fprintf_s(pHFile, "{\n");
		fprintf_s(pHFile, "\t::LeaveCriticalSection(&cs);\n");
		fprintf_s(pHFile, "}\n\n");

		fclose(pHFile);
	}
}

void ParseTable(const char* outFilePath, const char* inFlieName)
{
	TableFile inTableFile(inFlieName);
	ParseTable2H(outFilePath, inTableFile);
	ParseTable2CPP(outFilePath, inTableFile);
}

int main(int argc, char* argv[])
{
	
	//ParseTable("../../Common/log/", "./table/logConfig.csv");
	for (int i = 2; i < argc; i++)
	{ParseTable(argv[1], argv[i]);
	}

	return 0;
}

