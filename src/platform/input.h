#ifndef __INPUT_H__
#define __INPUT_H__

#include <wchar.h>
#include <conio.h>

#include "../core/definetion.h"

const int CTRL_S = 19;
const int TAB = 9;

const int ENTER = 13;
const int BACKSPACE = 8;

const int UP = -296;
const int DOWN = -304;
const int LEFT = -299;
const int RIGHT = -301;

const int DEL = -307;

/*
获取输入 不堵塞、回显
参考：https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/getch-getwch
*/
int input() noexcept;

#endif