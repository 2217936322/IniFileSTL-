#pragma once

#ifndef _FISH_INI_FILE_
#define _FISH_INI_FILE_

#include <string>
#include <vector>

namespace fish
{
	using namespace std;

	class IniFileSTL
	{
	public:
		explicit IniFileSTL(const string& fileName);
		~IniFileSTL(void);
		bool ReadFile(void);

		string ReadString(const string& section, const string& key, const string& value);
		int    ReadInt(const string& section, const string& key, int value);
		bool WriteString(const string& section, const string& key, const string& value);
		bool WriteInt(const string& section, const string& key, int value);
		bool RemoveSection(const string& section);
		bool RemoveKey(const string& section, const string& key);
		bool WriteFile(void);

		//获取当前可执行文件的路径（不包含EXE文件名）
		static string GetExeDir();

		//宽字节转多字节(wchar-char)
		static char* WcharToChar(const wchar_t* wp);

		//多字节转宽字节（char-wchar）
		static wchar_t* CharToWchar(const char* cp);
	private:

		static string Trim(const string& str);
		static string LTrim(const string& str);
		static string RTrim(const string& str);
	private:
		string m_fileName;
		vector<string> m_vctLine;
	};

}

#endif

//简单使用例子
//#include <stdio.h>
//#include <iostream>
//#include "IniFile.h"
//
//using namespace fish;
//
//int main(int argc, char **argv)
//{
//	IniFile ini("./test.ini");
//	cout<<ini.ReadString("system","ipaddr", "10.70.101.134")<<endl;
//	cout<<ini.ReadInt("system","tcpport", 7050 )<<endl;
//	return 0;
//
//
//string str;
//fish::IniFileSTL ini(fish::IniFileSTL::GetExeDir() + "\\cfglocal.ini");
//str = ini.ReadString("OccMarkShow", "Showflag", "0");
//}