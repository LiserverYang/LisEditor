#include "input.h"

int input() noexcept
{
    if (!_kbhit())
    {
        return INT_NO_INPUT;
    }
    else
    {
        wint_t in = _getwch();

        if (in == 0)
        {
            return -1 * _getwch();
        }
        else if (in == 0xE0)
        {
            return -1 * _getwch() - 0xE0;
        }
        else
        {
            return in;
        }
    }
}