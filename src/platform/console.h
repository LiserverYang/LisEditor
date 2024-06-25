#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include <windows.h>

/*
重新设置配置内控制台大小
*/
void reset_console_size();

/*
清空屏幕
*/
void cls(HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE));

#endif // __CONSOLE_H__