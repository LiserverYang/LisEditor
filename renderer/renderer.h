/*
 * 渲染器头文件
 *
 * The header file of renderer
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <vector>
#include <string>

#include "../style/style_template.h"

const wchar_t Keyword[][66] = {
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
    L"size_t"
};

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
    L'#'
};

/*
对于每个字符的值、颜色
*/
class PairWChar
{
public:
    wint_t ch;
    unsigned short cl;

    PairWChar() { ch = 32; cl = 0; }
    PairWChar(wint_t letter, unsigned short at = 0)
    {
        ch = letter;
        cl = at;
    }
};

bool is_keyword (std::wstring str);
bool is_operaotr(wchar_t str);
bool is_char(wchar_t c);
bool is_number(wchar_t c);

#define DELAY_TIME 1000

namespace Renderer
{
    /*
    C++ renderer
    */
    void rend(std::vector<std::vector<PairWChar>>& buffer);
}

#endif