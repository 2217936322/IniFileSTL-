#include <iostream>
#include <string>
#include "IniFileSTL.h"

int main()
{
    std::string str, str2;
    fish::IniFileSTL ini(fish::IniFileSTL::GetExeDir() + "\\test.ini");
    str = ini.ReadString("info", "key", "0");
    str2 = ini.ReadString("info", "path", "0");
    std::cout << str << std::endl;
    std::cout << str2 << std::endl;
    std::getchar();
}