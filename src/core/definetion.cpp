#include "definetion.h"

int max_line_size = 1;
bool exit_function should_exit = false;

template <class _Ty>
_Ty max(_Ty a, _Ty b)
{
    return (a > b ? a : b);
}

template <class _Ty>
_Ty min(_Ty a, _Ty b)
{
    return (a < b ? a : b);
}

std::vector<std::wstring> split(std::wstring &string)
{
    std::vector<std::wstring> result;
    std::wstring tmp = L"";

    for (int i = 0; i < string.size(); i++)
    {
        if (tmp[i] == L' ')
        {
            result.push_back(tmp);
            tmp = L"";
        }
        else if (i == string.size() - 1)
        {
            tmp += string[i];
            result.push_back(tmp);
            tmp = L"";
        }
        else
        {
            tmp += string[i];
        }
    }

    return result;
}