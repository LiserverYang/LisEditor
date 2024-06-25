#include "console.h"
#include "config/config.h"

void reset_console_size()
{
    // 获取宽高
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    config._width = csbi.dwSize.X;
    config._height = csbi.dwSize.Y;
}

void cls(HANDLE hConsole)
{
    COORD coordScreen = {0, 0}; /* 在这里我们将光标设置到正确的位置 */
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* 来获取Buffer的数据 */
    DWORD dwConSize;                 /* 当前Buffer可以容纳的字符数目 */
    /* 获取当前Buffer可以容纳的字符数量 */
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    /* 使用空格填充整个屏幕 */
    FillConsoleOutputCharacter(hConsole, (TCHAR)' ',
                               dwConSize, coordScreen, &cCharsWritten);
    /* 获取当前文本属性 */
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    /* 现在相应地设置Buffer的属性 */
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
                               dwConSize, coordScreen, &cCharsWritten);
    /* 把光标放到（0, 0） */
    SetConsoleCursorPosition(hConsole, coordScreen);

    return;
}