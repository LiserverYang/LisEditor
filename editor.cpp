/*
 * 这是一个简易的文本编辑器 使用WindowsAPI、STD标准库
 * MIT 开源协议
 *
 * This is a easy open-source text editor. Using WindowsAPI and STD standard library.
 * MIT licence.
 *
 * 额外的编译参数 -std=c++11 -Werror
 *
 * Extra compile arguments -std=c++11 -Werror
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#include <iostream>
#include <vector>
#include <string>
#include <io.h>
#include <conio.h>
#include <Windows.h>

// 对[std::string]的别称[str]
using str = std::string;

// 未输入
#define INT_NO_INPUT 0x00
#define UNKNOW_INPUT 0x01

// 该函数/函数引用的变量可能会退出程序
#define exit_function

/*
获取两个对象之中最大值
*/
template <class _Ty, class _Tz>
_Ty max(_Ty a, _Tz b)
{
    return (a > b ? a : b);
}

/*
获取两个对象之间最小值
*/
template <class _Ty, class _Tz>
_Ty min(_Ty a, _Tz b)
{
    return (a < b ? a : b);
}

/*
函数std::for_each()的简化
*/
template <class A, class B>
void for_each(A begin, A end, B func)
{
    for (auto it = begin; it != end; it++)
    {
        func(*it);
    }
}

/* 标准地报告API错误的宏 */
#define PERR(bSuccess, api)                                  \
    {                                                        \
        if (!(bSuccess))                                     \
            printf("%s:Error %d from %s \
on line %d\n",                                               \
                   __FILE__, GetLastError(), api, __LINE__); \
    }

/*
清空屏幕
*/
void cls(HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE))
{
    COORD coordScreen = {0, 0}; /* 在这里我们将光标设置到正确的位置 */
    BOOL bSuccess;
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* 来获取Buffer的数据 */
    DWORD dwConSize;                 /* 当前Buffer可以容纳的字符数目 */
    /* 获取当前Buffer可以容纳的字符数量 */
    bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
    PERR(bSuccess, "GetConsoleScreenBufferInfo");
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;
    /* 使用空格填充整个屏幕 */
    bSuccess = FillConsoleOutputCharacter(hConsole, (TCHAR)' ',
                                          dwConSize, coordScreen, &cCharsWritten);
    PERR(bSuccess, "FillConsoleOutputCharacter");
    /* 获取当前文本属性 */
    bSuccess = GetConsoleScreenBufferInfo(hConsole, &csbi);
    PERR(bSuccess, "ConsoleScreenBufferInfo");
    /* 现在相应地设置Buffer的属性 */
    bSuccess = FillConsoleOutputAttribute(hConsole, csbi.wAttributes,
                                          dwConSize, coordScreen, &cCharsWritten);
    PERR(bSuccess, "FillConsoleOutputAttribute");
    /* 把光标放到（0, 0） */
    bSuccess = SetConsoleCursorPosition(hConsole, coordScreen);
    PERR(bSuccess, "SetConsoleCursorPosition");
    return;
}

/*
程序的所有全局配置
_path : 编辑的文件路径
_readonly : 是否只读
_width : 控制台宽度
_height ： 控制台高度
*/
class Configtion
{
public:
    // 获取到的文件路径
    str _path = ".\\a.txt";
    // 是否只读
    bool _readonly = false;
    // 控制台的宽高
    int _width = 0, _height = 0;

    Configtion(str path = ".\\a.txt", bool readonly = false) : _path(path), _readonly(readonly) {}
};

// 全局地配置对象
Configtion config;

/*
当[FileIO]的read/write函数尝试读取/写入，但是没有权限时抛出的错误
*/
class NoPermissionsError : public std::exception
{
private:
    // 使错误抛出的文件路径
    str _path = "";

public:
    NoPermissionsError(str path) : _path(path) {}

    // 对std::exception类中virtual char const* what() const函数的实现
    virtual const char *what() const noexcept
    {
        return ("File '" + _path + "' hasn't permissions to read/write").c_str();
    }
};

/*
一个对文件的输入/输出控制类，包含了基本的读与写，以及对于权限的判断
*/
class FileIO
{
private:
    // 文件路径
    str _path = "";
    // 通过CreateFile函数获取的文件句柄
    HANDLE fileh;
    // 通过read_size函数获取的文件大小
    LARGE_INTEGER size;

public:
    FileIO() = delete;
    FileIO(str path) : _path(path)
    {
        // 不检查文件是否存在 但是不存在创建文件
        fileh = CreateFile(TEXT(_path.c_str()), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        // 获取文件大小
        read_size();
    }

    ~FileIO()
    {
        CloseHandle(fileh);
    }

    /*
    检查文件是否存在
    */
    bool check() noexcept
    {
        return access(_path.c_str(), 0x00) == 0;
    }

    /*
    检查文件是否有读权限
    */
    inline bool has_readpers() noexcept
    {
        return access(_path.c_str(), 0x02) == 0;
    }

    /*
    检查文件是否有写权限
    */
    inline bool has_writepers() noexcept
    {
        return access(_path.c_str(), 0x04) == 0;
    }

    /*
    读取文件大小
    */
    void read_size()
    {
        GetFileSizeEx(fileh, &size);
    }

    /*
    读取文件 返回字符串
    @exception NoPermissionsError
    */
    str read() /* throw(NoPermissionsError) */ noexcept(false)
    {
        if (!has_readpers())
        {
            throw NoPermissionsError(_path);
        }

        if (size.HighPart > 0)
        {
            // 文件太大，无法一次性读取
            return "";
        }

        DWORD fileSize32 = static_cast<DWORD>(size.LowPart);
        std::string content(fileSize32, '\0');

        DWORD bytesRead;
        if (!ReadFile(fileh, &content[0], fileSize32, &bytesRead, nullptr) || bytesRead != fileSize32)
        {
            // 处理读取文件内容失败的情况
            return "";
        }

        return content;
    }

    /*
    覆盖式地写入
    @exception NoPermissionsError
    */
    void write(str value) /* throw(NoPermissionsError) */ noexcept(false)
    {
        if (!has_writepers())
        {
            throw NoPermissionsError(_path);
        }
        SetFilePointer(fileh, 0, NULL, FILE_BEGIN);

        WriteFile(fileh, value.data(), value.length() * sizeof(str::value_type), nullptr, NULL);
    }

    inline str getpath() { return _path; }
};

/*
对于每个字符的值、颜色
*/
class PairWChar
{
public:
    wint_t ch;

    PairWChar() { ch = 32; }
    PairWChar(wint_t letter)
    {
        ch = letter;
    }
};

using WCharList = std::vector<PairWChar>;

/*
输出一行
*/
void output(WCharList list, WCharList::size_type begin, WCharList::size_type end)
{
    for (WCharList::size_type i = begin; i < end; i++)
    {
        std::wcout << (wchar_t)list[i].ch;
    }
}

/*
删除位置上某个字符
*/
void remove(WCharList &list, WCharList::size_type pos)
{
    list.erase(list.begin() + pos);
}

/*
比较是否相等
*/
inline bool operator!=(WCharList &lef, WCharList &rit)
{
    return lef.begin() != rit.begin();
}

/*
控制台缓冲区 全局唯一
应当注意的是，控制台缓冲区并不是仅限于对于控制台输出的文本的缓冲，它应当缓冲被编辑文件的全部内容，并且渲染时通过类内部渲染而不额外的从[FileIO]中同步应渲染的文本内容
*/
class Buffer
{
private:
    // cur_x , cur_y : 当前指针的行数、列数
    int cur_x = 0, cur_y = 0;
    // 缓冲区的名称 一般与文件名相同
    str _name = "New Buffer";
    // 缓冲区所绑定的[FileIO]
    FileIO _io = FileIO((str) ".\\a.txt");

    // 字符缓冲区
    std::vector<WCharList> text_buffer;

public:
    Buffer() = delete;
    Buffer(str name = "New Buffer") : _name(name), _io(config._path)
    {
        // 第一次同步 用于显示
        sync();
    }

    /*
    从[FileIO]中同步文本到缓冲区
    注意：当同步进行后，任何未保存的编辑都将被舍弃
    */
    void sync() noexcept
    {
        str filev = _io.read();

        std::string::size_type pos = _io.getpath().find_last_of('\\') + 1;
        _name = _io.getpath().substr(pos, _io.getpath().length() - pos);

        text_buffer.clear();

        str cur_v = "";

        for (str::size_type index = 0; index < filev.size(); index++)
        {
            if (filev[index] == '\r')
            {
                continue;
            }

            if (filev[index] == '\n' || index == filev.size() - 1)
            {
                if (index == filev.size() && filev[index] != '\n')
                {
                    cur_v += filev[index];
                }

                WCharList list = WCharList();
                list.resize(cur_v.size());

                for (str::size_type i = 0; i < cur_v.size(); i++)
                {
                    list.at(i) = PairWChar(cur_v[i]);
                }

                text_buffer.emplace_back(list);

                cur_v = "";
            }
            else
            {
                cur_v += filev.at(index);
            }
        }
    }

    /*
    从[FileIO]中保存文本
    */
    void save() & noexcept
    {
        // 如果只读 则不保存编辑
        if (config._readonly)
        {
            return;
        }

        str save_str = "";

        for_each(text_buffer.begin(), text_buffer.end(), [&](std::vector<WCharList>::reference it)
                 {
            for_each(it.begin(), it.end(), [&](WCharList::reference its)
            {
                if (its.ch != 0)
                    save_str += its.ch;
            });

            if (*(this->text_buffer.end() - 1) != it)
            {
                save_str += L'\r';
                save_str += L'\n';
            } });

        _io.write(save_str);

        // 在保存后需要主动同步 
        sync();
    }

    inline std::vector<WCharList> &getbuffer() { return text_buffer; }
    inline const str getname() { return _name; }
    inline int &getx() { return cur_x; }
    inline int &gety() { return cur_y; }
};

/*
解析程序的所有参数
*/
void parse_args(int argc, const char **argv, Configtion &config) noexcept
{
    for (int i = 1; i < argc; i++)
    {
        // arg
        if (argv[i][0] == '-')
        {
            if (0 == strcmp(argv[i], "-ro"))
            {
                config._readonly = true;
            }
        }
        else // file
        {
            config._path = argv[i];
        }
    }
}

/*
获取输入 不堵塞、回显
参考：https://learn.microsoft.com/zh-cn/cpp/c-runtime-library/reference/getch-getwch
*/
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

/*
移动光标
*/
void move_mouse_to(HANDLE handle, int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(handle, coord);
}

/*
判断一个值是否为特殊值
*/
#define is_curt(v) (v < 0)

/*
Buffer的非成员渲染函数
*/
void print(Buffer &buf)
{
    cls();                                                // 清空屏幕
    move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), 0, 0); // 设置输出光标

    std::vector<WCharList> &vec = buf.getbuffer();

    // TOP
    for (int i = 0; i < (config._width - buf.getname().size()) / 2; i++)
        std::wcout << ' ';

    std::wcout << buf.getname().c_str() << '\n';

    // MIDDLE
    int page_x = buf.getx() / config._height;
    int page_y = buf.gety() / config._width;

    for (int rx = page_x; rx < page_x + config._height - 2; rx++)
    {
        if (rx < vec.size())
        {
            std::wcout << rx + 1 << ' ';
            output(vec[rx], 0, min(vec[rx].size(), (WCharList::size_type)(config._width - 2)));
        }
        std::wcout << "\n";
    }

    std::wstring bustr = L"line[" + std::to_wstring(buf.getx() + 1) + L"] " + L" col[" + std::to_wstring(buf.gety() + 1) + L"] ";

    // BUTTOM
    for (int i = 0; i < (config._width - bustr.size()) / 2; i++)
        std::wcout << ' ';
    std::wcout << bustr;

    // 刷新输出缓冲区 大坑:_:
    std::wcout.flush();
}

/*
程序是否应该退出 作为主循环的退出条件
*/
bool exit_function should_exit = false;

/*
编辑器的主体
*/
void exit_function tick_loop(Buffer &buf)
{
    // 获取输入
    int key = input();

    // 无输入 不更新
    if (key == INT_NO_INPUT)
    {
        return;
    }

    // 输入的是普通键
    if (!is_curt(key))
    {
        // 如果是普通字符
        if (key >= 27)
        {
            buf.getbuffer()[buf.getx()].insert(buf.getbuffer()[buf.getx()].begin() + buf.gety(), PairWChar(key));
            buf.gety() += 1;
        }
        // 如果是特殊字符
        {
            switch (key)
            {
            case 3:  // CTRL + C
                buf.save();
                should_exit = true;
            case 19: // CTRL + S
                buf.save();
                break;
            case 13: // ENTER
                buf.getbuffer().insert(buf.getbuffer().begin() + buf.getx() + 1, {0});
                buf.getx() += 1;
                buf.gety() = 0;
                break;
            case 8: // BACKSPACE
                if (buf.gety() == 0 && buf.getx() > 0)
                {
                    buf.getbuffer()[buf.getx() - 1].insert(buf.getbuffer()[buf.getx() - 1].end(), buf.getbuffer()[buf.getx()].begin(), buf.getbuffer()[buf.getx()].end());
                    buf.getbuffer().erase(buf.getbuffer().begin() + buf.getx());
                    buf.getx() -= 1;
                    buf.gety() = buf.getbuffer()[buf.getx()].size() - 1;
                }
                else if (buf.gety() > 0)
                {
                    remove(buf.getbuffer()[buf.getx()], buf.gety() - 1);
                    buf.gety() -= 1;
                }
                break;
            default: // 不关注的特殊键
                break;
            }
        }
    }
    // 输入的是特殊键
    else
    {
        switch (key)
        {
        case -296: // UP
            if (buf.getx() > 0)
            {
                buf.getx() -= 1;
                buf.gety() = min(buf.gety(), buf.getbuffer()[buf.getx()].size() - 1);
            }
            break;
        case -304: // DOWN
            if (buf.getx() < (buf.getbuffer().size() - 1))
            {
                buf.getx() += 1;
                buf.gety() = min(buf.gety(), buf.getbuffer()[buf.getx()].size() - 1);
            }
            break;
        case -299: // LEFT
            if (buf.gety() > 0)
            {
                buf.gety() -= 1;
            }
            break;
        case -301: // RIGHT
            if (buf.gety() <= (buf.getbuffer()[buf.getx()].size() - 1))
            {
                buf.gety() += 1;
            }
            break;
        case -307: // DEL
            break;
        default: // 不关注的特殊键 无需处理
            break;
        }
    }

    print(buf);
    move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), buf.gety() + 2, buf.getx() + 1);
}

/*
程序的主入口
*/
int exit_function main(int argc, const char **argv)
{
    // Text editor

    /*
     SCREEN:

     [Middle]BufferName[/Middle] <- Top
     1 Code
     2 Code
     3 Code
     4 Code                      <- Middle
     5 Code
     6 Code
     ...
     [Middle]line[0/0]  col[0/0][/Middle] <- Buttom
     */

    // 关闭io同步提升渲染时间
    std::ios::sync_with_stdio(0);
    std::wcin.tie(0);
    std::wcout.tie(0);

    // 解析参数
    parse_args(argc, argv, config);

    // 获取宽高
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    config._width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    config._height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;

    // 初始清空屏幕
    cls();

    // 创建buffer
    Buffer buf("New Buffer");

    // 第一次渲染输出。循环内渲染是惰性的。第一次不输出可能会导致一段时间内没有文字
    print(buf);

    // 开始主循环 主循环每次调用tick函数 直到CTRL+C退出
    while (!should_exit)
    {
        // 调用tick函数
        tick_loop(buf);
    }

    return 0;
}