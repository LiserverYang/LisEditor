/*
 * 渲染器头文件
 *
 * The header file of renderer
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#pragma once

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <vector>
#include <string>

/*
 * If you want to change the style, here.
 * 如果你想要修改样式，在这里
 */
#include "../style/style_template.h"

#include <iostream>

const wchar_t Keyword[][69] = {
    L"asm",
    L"do",
    L"if",
    L"return",
    L"typedef",
    L"auto",
    L"double",
    L"inline",
    L"short",
    L"typeid",
    L"bool",
    L"dynamic_cast",
    L"int",
    L"signed",
    L"typename",
    L"break",
    L"else",
    L"long",
    L"sizeof",
    L"union",
    L"case",
    L"enum",
    L"mutable",
    L"static",
    L"unsigned",
    L"catch",
    L"explicit",
    L"namespace",
    L"static_cast",
    L"using",
    L"char",
    L"export",
    L"new",
    L"struct",
    L"virtual",
    L"class",
    L"extern",
    L"operator",
    L"switch",
    L"void",
    L"const",
    L"false",
    L"private",
    L"template",
    L"volatile",
    L"const_cast",
    L"float",
    L"protected",
    L"this",
    L"wchar_t",
    L"continue",
    L"for",
    L"public",
    L"throw",
    L"while",
    L"default",
    L"friend",
    L"register",
    L"true",
    L"delete",
    L"goto",
    L"reinterpret_cast",
    L"try",
    L"noexcept",
    L"operator",
    L"size_t",
    L"final",
    L"requires",
    L"concept"};

const wchar_t Operator[] = {
    L'+',
    L'-',
    L'*',
    L'/',
    L'\'',
    L'"',
    L'\\',
    L'*',
    L':',
    L'%',
    L'!',
    L'=',
    L'>',
    L'<',
    L'&',
    L'|',
    L'<',
    L'>',
    L'^',
    L'(',
    L')',
    L'[',
    L']',
    L'{',
    L'}',
    L'.',
    L'$',
    L'#'};

/*
对于每个字符的值、颜色
*/
class PairWChar
{
public:
    wint_t ch;
    unsigned short cl;

    PairWChar()
    {
        ch = 32;
        cl = 0;
    }

    PairWChar(wint_t letter, unsigned short at = 0)
    {
        ch = letter;
        cl = at;
    }
};

// 定义待输出行/缓冲行
using WCharList = std::vector<PairWChar>;

/*
比较两个待输出字符是否相等
*/
bool operator==(PairWChar a, PairWChar b);

/*
比较两个待输出字符是否不相等
*/
bool operator!=(PairWChar a, PairWChar b);

/*
字符是否是中文
*/
bool IsChineseChar(PairWChar ch);

/*
输出一行
*/
void output(WCharList list, WCharList::size_type begin, WCharList::size_type end);

/*
删除位置上某个字符
*/
void remove(WCharList &list, WCharList::size_type pos);



namespace Renderer
{
    /*
    C++ renderer
    */
    void rend(std::vector<std::vector<PairWChar>> &buffer);
}

#endif // __RENDERER_H__