#include <Windows.h>
#include "IniFileSTL.h"
#include <fstream>
#include <sstream>//std::stringstream(推荐使用)
#include <string>


namespace fish
{

	IniFileSTL::IniFileSTL(const string& fileName)
		:m_fileName(fileName)
	{
		ReadFile();
	}

	IniFileSTL::~IniFileSTL(void)
	{
		WriteFile();
	}

	//获取当前可执行文件的路径（不包含EXE文件名）
	string IniFileSTL::GetExeDir()
	{
		wchar_t exeFullPath[MAX_PATH]; // Full path  
		string StrPath = " ";
		GetModuleFileName(NULL, exeFullPath, MAX_PATH);

		StrPath = (string)WcharToChar(exeFullPath);    // Get full path of the file   
		int leng = StrPath.length();
		int pos = StrPath.find_last_of('\\', StrPath.length());
		StrPath = StrPath.substr(0, pos);
		return StrPath.substr(0, pos);  // Return the directory without the file name
	}

	//宽字节转多字节(wchar-char)
	char* IniFileSTL::WcharToChar(const wchar_t* wp)
	{
		locale old_loc = locale::global(locale(""));

		size_t len = wcslen(wp) * sizeof(wchar_t);
		size_t converted = 0;
		char* CStr;
		CStr = (char*)malloc(len);
		wcstombs_s(&converted, CStr, len, wp, _TRUNCATE);

		locale::global(old_loc);
		return CStr;
	}

	//多字节转宽字节（char-wchar）
	wchar_t* IniFileSTL::CharToWchar(const char* cp)
	{
		locale old_loc = locale::global(locale(""));

		size_t len = strlen(cp) * sizeof(char);
		size_t converted = 0;
		wchar_t* WStr;
		WStr = (wchar_t*)malloc(2 * len);
		mbstowcs_s(&converted, WStr, len, cp, _TRUNCATE);

		locale::global(old_loc);
		return WStr;
	}

	bool IniFileSTL::ReadFile(void)
	{
		ifstream in(m_fileName.c_str());
		bool bopen = in.is_open();
		string line;
		while (getline(in, line))
		{
			m_vctLine.push_back(line);
		}
		return true;
	}

	string IniFileSTL::ReadString(const string& section, const string& key, const string& value)
	{
		for (size_t i = 0; i < m_vctLine.size(); ++i)
		{
			string& section_line = m_vctLine[i];
			size_t sec_begin_pos = section_line.find('[');
			if (sec_begin_pos == string::npos || sec_begin_pos != 0)
			{
				continue;
			}
			size_t sec_end_pos = section_line.find(']', sec_begin_pos);
			if (sec_end_pos == string::npos)
			{
				continue;
			}

			if (section != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)))
			{
				continue;
			}

			//find key, ????????±ê????i
			for (++i; i < m_vctLine.size(); ++i)
			{
				string& key_line = m_vctLine[i];
				size_t sec_pos = key_line.find('[');
				if (sec_pos != string::npos && sec_pos == 0)
				{
					--i;  //reback a step,find again
					break;//the line is section line
				}

				size_t begin_key_pos = key_line.find(key);
				if (begin_key_pos == string::npos)
				{
					continue;
				}
				size_t equal_pos = key_line.find('=', begin_key_pos);
				if (equal_pos == string::npos)
				{
					continue;
				}
				if (key != Trim(key_line.substr(begin_key_pos, equal_pos - begin_key_pos)))
				{
					continue;
				}

				size_t comment_pos = key_line.find("#", equal_pos + 1);
				if (comment_pos != string::npos)
				{
					//????×???
					return Trim(key_line.substr(equal_pos + 1, comment_pos - equal_pos - 1));
				}

				return Trim(key_line.substr(equal_pos + 1));
			}
		}

		return value;
	}

	bool IniFileSTL::WriteString(const string& section, const string& key, const string& value)
	{
		for (size_t i = 0; i < m_vctLine.size(); ++i)
		{
			string& section_line = m_vctLine[i];
			size_t sec_begin_pos = section_line.find('[');
			if (sec_begin_pos == string::npos || sec_begin_pos != 0)
			{
				continue;
			}
			size_t sec_end_pos = section_line.find(']', sec_begin_pos);
			if (sec_end_pos == string::npos)
			{
				continue;
			}
			if (section != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)))
			{
				continue;
			}

			//find key, ????????±ê????i
			for (++i; i < m_vctLine.size(); ++i)
			{
				string& key_line = m_vctLine[i];
				size_t sec_pos = key_line.find('[');
				if (sec_pos != string::npos && sec_pos == 0)
				{
					--i;  //reback a step,find again
					break;//the line is section line
				}

				size_t begin_key_pos = key_line.find(key);
				if (begin_key_pos == string::npos)
				{
					continue;
				}
				size_t equal_pos = key_line.find('=', begin_key_pos);
				if (equal_pos == string::npos)
				{
					continue;
				}
				if (key != Trim(key_line.substr(begin_key_pos, equal_pos - begin_key_pos)))
				{
					continue;
				}

				size_t comment_pos = key_line.find("#", equal_pos + 1);
				string new_line = key_line.substr(0, equal_pos + 1) + value;
				if (comment_pos != string::npos)
				{
					new_line += key_line.substr(comment_pos);
				}
				key_line = new_line;

				return true;
			}

			//??????key
			m_vctLine.insert(m_vctLine.begin() + i, key + "=" + value);
			return true;
		}

		//section??key????????

		m_vctLine.insert(m_vctLine.end(), "");
		m_vctLine.insert(m_vctLine.end(), "[" + section + "]");
		m_vctLine.insert(m_vctLine.end(), key + "=" + value);
		return true;
	}

	bool IniFileSTL::RemoveSection(const string& section)
	{
		for (size_t i = 0; i < m_vctLine.size(); ++i)
		{
			string& section_line = m_vctLine[i];
			size_t sec_begin_pos = section_line.find('[');
			if (sec_begin_pos == string::npos || sec_begin_pos != 0)
			{
				continue;
			}
			size_t sec_end_pos = section_line.find(']', sec_begin_pos);
			if (sec_end_pos == string::npos)
			{
				continue;
			}
			if (section != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)))
			{
				continue;
			}

			//??????section
			size_t del_begin = i;
			for (++i; i < m_vctLine.size(); ++i)
			{
				string& next_section = m_vctLine[i];
				size_t next_pos = next_section.find('[');
				if (next_pos == string::npos || next_pos != 0)
				{
					continue;
				}

				break;
			}
			m_vctLine.erase(m_vctLine.begin() + del_begin, m_vctLine.begin() + i);
			return true;
		}
		return false;
	}

	bool IniFileSTL::RemoveKey(const string& section, const string& key)
	{
		for (size_t i = 0; i < m_vctLine.size(); ++i)
		{
			string& section_line = m_vctLine[i];
			size_t sec_begin_pos = section_line.find('[');
			if (sec_begin_pos == string::npos || sec_begin_pos != 0)
			{
				continue;
			}
			size_t sec_end_pos = section_line.find(']', sec_begin_pos);
			if (sec_end_pos == string::npos)
			{
				continue;
			}
			if (section != Trim(section_line.substr(sec_begin_pos + 1, sec_end_pos - sec_begin_pos - 1)))
			{
				continue;
			}

			//??????section
			for (++i; i < m_vctLine.size(); ++i)
			{
				string& key_line = m_vctLine[i];
				size_t key_pos = key_line.find(key);
				if (key_pos == string::npos || key_pos != 0)
				{
					continue;
				}
				else
				{
					m_vctLine.erase(m_vctLine.begin() + i);
					return true;
				}
			}
		}
		return false;
	}

	bool IniFileSTL::WriteFile(void)
	{
		ofstream out(m_fileName.c_str());
		for (size_t i = 0; i < m_vctLine.size(); ++i)
		{
			out << m_vctLine[i] << endl;
		}
		return true;
	}

	int IniFileSTL::ReadInt(const string& section, const string& key, int value)
	{
		string str = ReadString(section, key, "");
		if ("" == str)
		{
			return value;
		}

		std::stringstream in(str.c_str());
		int ret = 0;
		in >> ret;
		return ret;
	}

	bool IniFileSTL::WriteInt(const string& section, const string& key, int value)
	{
		std::stringstream out;
		out << value;
		//char buffer[64] = "";
		//itoa( value, buffer, 10 );
		return WriteString(section, key, out.str());
	}

	string IniFileSTL::LTrim(const string& str)
	{
		size_t pos = 0;
		while (pos != str.size())
		{
			if (' ' == str[pos])
			{
				++pos;
			}
			else
			{
				break;
			}
		}

		return str.substr(pos);
	}

	string IniFileSTL::RTrim(const string& str)
	{
		size_t pos = str.size() - 1;
		while (pos >= 0)
		{
			if (' ' == str[pos])
			{
				--pos;
			}
			else
			{
				break;
			}
		}

		return str.substr(0, pos + 1);
	}

	string IniFileSTL::Trim(const string& str)
	{
		return LTrim(RTrim(str));
	}

}
