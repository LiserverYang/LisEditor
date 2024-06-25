#ifndef __DEFINETION_H__
#define __DEFINETION_H__

#include <Windows.h>
#include <string>
#include <vector>
#include <iostream>

// 对[std::string]的别称[str]
using str = std::string;

// 未输入
#define INT_NO_INPUT 0x00

// 该函数/函数引用的变量可能会导致程序退出
#define exit_function

// 按键输入后处理缓冲区的函数
#define key_function

// 行号所占的最大宽度
extern int max_line_size;

// 程序是否应该退出 作为主循环的退出条件
extern bool exit_function should_exit;

/*
获取两个对象之中最大值
*/
template <class _Ty>
_Ty max(_Ty a, _Ty b);

/*
获取两个对象之间最小值
*/
template <class _Ty>
_Ty min(_Ty a, _Ty b);

/*
函数std::for_each的简化
*/
template <class A, class B>
void for_each(A begin, A end, B func)
{
    for (A it = begin; it != end; it++)
    {
        func(*it);
    }
}

/*
分割字符串
*/
std::vector<std::wstring> split(std::wstring &string);

#endif