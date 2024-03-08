/*
 * 渲染器实现
 *
 * The renderer
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#include "./renderer.h"

#include <time.h>

bool is_keyword(std::wstring str)
{
    for (int i = 0; i < (int)sizeof(Keyword) / (int)sizeof(Keyword[0]); i++)
    {
        if (str == Keyword[i])
            return true;
    }
    return false;
}

bool is_operaotr(wchar_t str)
{
    for (int i = 0; i < (int)sizeof(Operator) / (int)sizeof(Operator[0]); i++)
    {
        if (str == Operator[i])
            return true;
    }
    return false;
}

bool is_char(wchar_t c)
{
    return (c >= L'a' && c <= L'z') || (c >= L'A' && c <= L'Z') || c == L'_';
}

bool is_number(wchar_t c)
{
    return c >= L'0' && c <= L'9';
}

/*
C++ renderer
*/
void Renderer::rend(std::vector<std::vector<PairWChar>> &buffer)
{
    static clock_t last_called_time;
    static bool called = false;

    clock_t now_called_time = clock();

    if (!called)
    {
        called = true;
    }

    else if ((now_called_time - last_called_time) * 1000 / CLOCKS_PER_SEC < DELAY_TIME)
    {
        return;
    }

    last_called_time = now_called_time;

    if (buffer.empty())
        return;

    for (int i = 0; i < buffer.size(); i++)
    {
        int j = 0;

        if (buffer[i].empty())
            continue;

        while (j < buffer[i].size())
        {
            if (buffer[i][j].ch == L' ')
            {
                j++;
                continue;
            }

            if (is_operaotr(buffer[i][j].ch))
            {
                buffer[i][j].cl = style::S_OPERATOR_COLOR;

                if (buffer[i][j].ch == L'\'')
                {
                    j++;

                    while (buffer[i][j].ch != L'\'' && j < buffer[i].size())
                    {
                        buffer[i][j].cl = style::S_STRING_COLOR;
                        j++;
                    }

                    buffer[i][j].cl = style::S_OPERATOR_COLOR;
                    j++;
                }

                else if (buffer[i][j].ch == L'"')
                {
                    j++;

                    while (buffer[i][j].ch != L'"' && j < buffer[i].size())
                    {
                        buffer[i][j].cl = style::S_STRING_COLOR;
                        j++;
                    }

                    buffer[i][j].cl = style::S_OPERATOR_COLOR;
                    j++;
                }

                else if ((buffer[i][j].ch == L'/' && buffer[i][j + 1].ch == L'/'))
                {
                    for (std::size_t x = j; x < buffer[i].size(); x++)
                    {
                        buffer[i][x].cl = style::S_OPERATOR_COLOR;
                    }

                    break;
                }

                else if (buffer[i][j].ch == L'#')
                {
                    while (buffer[i][j].ch != L' ' && j < buffer[i].size())
                    {
                        buffer[i][j].cl = style::S_OPERATOR_COLOR;
                        j++;
                    }
                }

                else
                    j++;

                continue;
            }

            if (is_char(buffer[i][j].ch))
            {
                int begin = j;
                std::wstring charv = L"";

                while (is_char(buffer[i][j].ch) && j < buffer[i].size())
                {
                    charv += buffer[i][j].ch;
                    j++;
                }

                if (is_keyword(charv))
                {
                    for (int v = begin; v < j; v++)
                    {
                        buffer[i][v].cl = style::S_KEYWORD_COLOR;
                    }
                }

                if (j == begin)
                {
                    j++;
                }

                continue;
            }

            if (is_number(buffer[i][j].ch))
            {
                buffer[i][j].cl = style::S_NUMBER_COLOR;

                j++;
                continue;
            }

            j++;
        }
    }
}