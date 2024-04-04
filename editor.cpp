/*
 * 这是一个简易的文本编辑器 使用WindowsAPI、STL标准模板库
 * MIT 开源协议
 *
 * This is a simple text editor. Using WindowsAPI and STL standard template library.
 * MIT licence.
 *
 * 额外的编译参数 -std=c++11 -Werror
 *
 * Extra compile arguments -std=c++11 -Werror
 *
 * C++版本：c++11或以上
 * O2优化应不影响程序正常运行
 *
 * C++ version: c++11 or more.
 * O2 does not change the application to run.
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#ifndef __cplusplus
#error This is a c++ project.
#endif

#include <memory>
#include <cmath>
#include <locale>
#include <vector>
#include <string>
#include <cstring>
#include <codecvt>
#include <iostream>

#include "renderer/renderer.h"

#include "command/factory.h"

#if defined(_WIN32)
#include <io.h>
#include <conio.h>
#include <Windows.h>
#else
#error This project is only support windows.
#endif

// 对[std::string]的别称[str]
using str = std::string;

// 未输入
#define INT_NO_INPUT 0x00

// 该函数/函数引用的变量可能会导致程序退出
#define exit_function

// 按键输入后处理缓冲区的函数
#define key_function

// 行号所站的最大宽度
int max_line_size = 1;

// 程序是否应该退出 作为主循环的退出条件
bool exit_function should_exit = false;

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
清空屏幕
*/
void cls(HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE))
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

/*
程序的所有全局配置
_path     : 编辑的文件路径
_readonly : 是否只读
_width    : 控制台宽度
_height   : 控制台高度
*/
class Configtion
{
public:
    // 获取到的文件路径
    str _path = "";
    // 是否只读
    bool _readonly = false;
    // 是否创建文件
    bool _create = false;
    // 控制台的宽高
    int _width = 0, _height = 0;

    Configtion(str path = "", bool readonly = false) : _path(path), _readonly(readonly) {}
};

// 全局的配置对象
Configtion config;

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
            else if (0 == strcmp(argv[i], "-readonly"))
            {
                config._readonly = true;
            }
            else if (0 == strcmp(argv[i], "-create"))
            {
                config._create = true;
            }
        }
        else // file
        {
            config._path = argv[i];
        }
    }
}

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

    // 无效IO
    bool error_io = false;

public:
    FileIO() = delete;
    FileIO(str path) : _path(path)
    {
        if (path == "")
        {
            error_io = true;
            return;
        }

        if (check() || !check() && config._create)
        {
            fileh = CreateFileW(std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>>().from_bytes(_path).c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
            // 获取文件大小
            read_size();
        }
        else if (!check())
        {
            config._readonly = true;
        }
    }

    FileIO(int v)
    {
        error_io = true;
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
        return access(_path.c_str(), F_OK) != -1;
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
    str read() noexcept
    {
        if (-1 == access(_path.c_str(), 0) || error_io)
            return "";

        if (!has_readpers())
            return "";

        // 文件太大，无法一次性读取
        if (size.HighPart > 0)
            return "";

        DWORD fileSize32 = static_cast<DWORD>(size.LowPart);
        std::string content(fileSize32, '\0');

        DWORD bytesRead;

        // 处理读取文件内容失败的情况
        if (!ReadFile(fileh, &content[0], fileSize32, &bytesRead, nullptr) || bytesRead != fileSize32)
            return "";

        return content;
    }

    /*
    覆盖式地写入
    @exception NoPermissionsError

    TODO Bugs waiting for fix!
    - Can not save the texts
    */
    void write(str value) noexcept
    {
        if (config._readonly || error_io)
            return;

        if (!has_writepers())
            return;

        SetFilePointer(fileh, 0, NULL, FILE_BEGIN);

        WriteFile(fileh, value.data(), value.length() * sizeof(str::value_type), nullptr, NULL);

        LARGE_INTEGER moveSize;
        moveSize.QuadPart = value.size();

        SetFilePointerEx(fileh, moveSize, NULL, FILE_BEGIN);
        SetEndOfFile(fileh);
    }

    /*
    获取文件路径
    */
    inline str getpath() { return _path; }
};

// 定义待输出行/缓冲行
using WCharList = std::vector<PairWChar>;

/*
比较两个待输出字符是否相等
*/
bool operator==(PairWChar a, PairWChar b)
{
    return (a.ch == b.ch);
}

/*
比较两个待输出字符是否不相等
*/
bool operator!=(PairWChar a, PairWChar b)
{
    return (a.ch != b.ch);
}

/*
字符是否是中文
*/
inline bool IsChineseChar(PairWChar ch)
{
    return (ch.ch >= 0x4E00) && (ch.ch <= 0x9FA5);
}

/*
输出一行
*/
void output(WCharList list, WCharList::size_type begin, WCharList::size_type end)
{
    for (WCharList::size_type i = begin; i < end; i++)
    {
        std::wcout << L"\033[" << list[i].cl << L"m" << (wchar_t)list[i].ch << L"\033[0m";
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
    FileIO _io = FileIO((str)_name);

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

        std::string::size_type pos = std::max(_io.getpath().find_last_of('\\') + 1, _io.getpath().find_last_of('/') + 1);
        str new_name = _io.getpath().substr(pos, _io.getpath().length() - pos);

        if (new_name != "")
            _name = new_name;

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
                if (index == filev.size() - 1 && filev[index] != '\n')
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

        if (filev.size() == 0)
        {
            text_buffer.emplace_back();
        }

        max_line_size = std::to_string(text_buffer.size()).size();
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
    }

    inline std::vector<WCharList> &getbuffer() { return text_buffer; }
    inline const str getname() { return _name; }
    inline int &getx() { return cur_x; }
    inline int &gety() { return cur_y; }
};

// 是否处于命令模式
bool input_command = false;
// 输入的命令
std::wstring command_inputed = L"";
// 当前指向的y
int command_cur_y = 0;

class command_save : public command::command
{
public:
    virtual int command_id() override {return 1;}

    virtual void on_called(ContextManager& context, std::vector<std::wstring> &argments) override
    {
        context.get_by_id<Buffer>("buffer")->save();
    }

    virtual bool check(std::wstring command_name_str) override
    {
        return command_name_str == L"save";
    }
};

class command_quit : public command::command
{
public:
    virtual int command_id() override {return 2;}

    virtual void on_called(ContextManager& context, std::vector<std::wstring> &argments) override
    {
        *(context.get_by_id<bool>("should_exit")) = true;
    }

    virtual bool check(std::wstring command_name_str) override
    {
        return command_name_str == L"quit";
    }
};

class command_save_quit : public command::command
{
public:
    virtual int command_id() override {return 3;}

    virtual void on_called(ContextManager& context, std::vector<std::wstring> &argments) override
    {
        context.get_by_id<Buffer>("buffer")->save();
        *(context.get_by_id<bool>("should_exit")) = true;
    }

    virtual bool check(std::wstring command_name_str) override
    {
        return command_name_str == L"sq";
    }
};

void command::factory::init()
{
    regist_command(std::make_shared<command_save>());
    regist_command(std::make_shared<command_quit>());
    regist_command(std::make_shared<command_save_quit>());
}

std::vector<std::wstring> split(std::wstring& string)
{
    std::vector<std::wstring> result;
    std::wstring tmp = L"";

    for(int i = 0; i < string.size(); i++)
    {
        if (tmp[i] == L' ')
        {
            result.push_back(tmp);
            tmp = L"";
        }
        else if(i == string.size() - 1)
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

/*
解析命令
*/
void exit_function key_function command_parser(std::wstring &command)
{
    std::vector<std::wstring> command_list = split(command);

    if (!command_list.empty())
    for(int i = 0; i < command::g_command_factory.list.size(); i++)
    {
        if (command::g_command_factory.list[i]->check(command_list[0]))
        {
            command::g_command_factory.list[i]->on_called(context, command_list);

            break;
        }
    }

    // 命令在解析之后清空
    command = L"";
    command_cur_y = 0;
    input_command = false;
}

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

int last_page_x = 0;
int last_page_y = 0;

inline std::size_t get_max_y(int page_y = last_page_y)
{
    return (last_page_y + 1) * (config._width - 1 - max_line_size);
}

inline std::size_t get_min_y(int page_y = last_page_y)
{
    return last_page_y * (config._width - 1 - max_line_size);
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

void move_to_command_text()
{
    move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), command_cur_y, config._height - 1);
}

/*
移动到某个字符
*/
void move_to_text(Buffer &buf, int x, int y)
{
    int real_x = 1 + max_line_size;
    int real_y = x;

    for (int i = get_min_y(); i < y; i++)
    {
        if (IsChineseChar(buf.getbuffer()[x][i]))
        {
            real_x += 2;
        }
        else
        {
            real_x += 1;
        }
    }

    move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), real_x, (real_y % (config._height - 2)) + 1);
}

/*
格式化输出数字，按照宽度
*/
inline void wint_opt(int size, std::wint_t value)
{
    int vsize = std::to_wstring(value).size();

    if (vsize > size)
    {
        std::wcout << std::to_wstring(value);
    }
    else
    {
        for (int i = 0; i < size - vsize; i++)
        {
            std::wcout << ' ';
        }

        std::wcout << std::to_wstring(value);
    }
}

/*
更新底部
*/
inline void update_buttom(Buffer &buf, bool move_mouse = true)
{
    if (move_mouse)
        move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), 0, config._height - 1);

    std::wstring bustr = L"line[" + std::to_wstring(buf.getx() + 1) + L"] " + L" col[" + std::to_wstring(buf.gety() + 1) + L"] ";

    int i = 0;

    const int space_size = (input_command) ? ((config._width - bustr.size()) - 1) : ((config._width - bustr.size()) / 2);

    if (command_inputed.size() != 0)
    {
        if (command_inputed.size() - 1 < space_size)
        {
            std::wcout << command_inputed;
        }

        i = command_inputed.size();
    }

    for (; i < space_size; i++)
        std::wcout << ' ';

    std::wcout << bustr;

    for (i = i + bustr.size(); i < config._width; i++)
        std::wcout << ' ';

    std::wcout.flush();
}

/*
Buffer的非成员渲染函数
*/
void print(Buffer &buf)
{
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

    cls(); // 清空屏幕

    move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), 0, 0); // 设置输出光标

    std::vector<WCharList> &vec = buf.getbuffer();

    const str name = buf.getname();

    // TOP
    for (int i = 0; i < (config._width - name.size()) / 2; i++)
        std::wcout << ' ';

    std::wcout << name.c_str() << '\n';

    // MIDDLE
    int page_x = buf.getx() / (config._height - 2);
    int page_y = buf.gety() / (config._width - 1 - max_line_size);

    last_page_x = page_x;
    last_page_y = page_y;

    for (int rx = 0; rx < config._height - 2; rx++)
    {
        if ((rx + page_x * (config._height - 2)) < vec.size())
        {
            const std::size_t max_size = get_max_y();
            const std::size_t min_size = (page_y) * (config._width - 1 - max_line_size);

            wint_opt(max_line_size, rx + page_x * (config._height - 2) + 1);
            std::wcout << ' ';

            output(vec[rx + page_x * (config._height - 2)], min_size, std::min(vec[rx + page_x * (config._height - 2)].size(), max_size));
        }
        std::wcout << "\n";
    }

    // BUTTOM
    update_buttom(buf, false);

    std::wcout.flush();
}

/*
添加一个文字
*/
inline void key_function add(Buffer &buf, int key)
{
    buf.getbuffer()[buf.getx()].insert(buf.getbuffer()[buf.getx()].begin() + buf.gety(), PairWChar(key));
    buf.gety() += 1;

    move_to_text(buf, buf.getx(), 0);

    int cur_page_y = buf.gety() / (config._width - 1 - max_line_size);

    if (cur_page_y != last_page_y)
    {
        last_page_y = cur_page_y;

        print(buf);
        return;
    }

    output(buf.getbuffer()[buf.getx()], get_min_y(cur_page_y), std::min(buf.getbuffer()[buf.getx()].size(), get_max_y(cur_page_y)));

    std::wcout.flush();

    update_buttom(buf);
}

/*
编辑器逻辑的主体
*/
void exit_function tick_loop(Buffer &buf)
{
    if (input_command)
        move_to_command_text();
    else
        move_to_text(buf, buf.getx(), buf.gety());

    // 获取输入
    int key = input();

    // 无输入 不更新
    if (key == INT_NO_INPUT)
    {
        return;
    }

    int &x = buf.getx();
    int &y = buf.gety();

    std::vector<WCharList> &tbuf = buf.getbuffer();

    // 输入的是普通键
    if (key > 0)
    {
        // 如果是普通字符
        if (key > 27)
        {
            if (input_command)
            {
                command_inputed += (char)key;
                command_cur_y++;
                update_buttom(buf);
            }
            else
            {
                add(buf, key);
                Renderer::rend(tbuf);
            }
            return;
        }
        // ESC
        else if (key == 27)
        {
            if (input_command)
            {
                command_parser(command_inputed);

                input_command = false;
                command_cur_y = 0;
                command_inputed = L"";
            }
            else
            {
                input_command = true;
            }

            update_buttom(buf);

            return;
        }
        // 如果是特殊字符
        else
        {
            switch (key)
            {
            case CTRL_S: // CTRL + S
                buf.save();
                return;
            case ENTER: // ENTER
            {
                if (input_command)
                {
                    input_command = !input_command;

                    command_parser(command_inputed);

                    update_buttom(buf);

                    return;
                }

                tbuf.emplace(tbuf.begin() + x + 1);

                std::size_t tmpv = 0;

                if (!tbuf[x].empty())
                    for (std::size_t i = 0; i <= y; i++, tmpv++)
                    {
                        if (tbuf[x][i] != 32)
                            break;

                        tbuf[x + 1].insert(tbuf[x + 1].begin() + i, PairWChar(' '));
                    }

                if (tbuf[x].size() != 0)
                {
                    std::size_t count = 0;

                    for (std::size_t i = y; i < tbuf[x].size(); i++, count++)
                    {
                        tbuf[x + 1].push_back(tbuf[x][i]);
                    }

                    for (std::size_t i = 0; i < count; i++)
                    {
                        tbuf[x].pop_back();
                    }
                }

                x += 1;
                y = tmpv;

                max_line_size = std::to_string(tbuf.size()).size();

                break;
            }
            case BACKSPACE: // BACKSPACE
                if (input_command)
                {
                    if (command_inputed.size() > 0 && command_cur_y > 0)
                    {
                        command_inputed.erase(command_inputed.begin() + command_cur_y - 1);
                        command_cur_y--;
                    }

                    update_buttom(buf);

                    return;
                }

                if (y == 0 && x > 0)
                {
                    for (std::size_t i = 0; i < tbuf[x].size(); i++)
                    {
                        tbuf[x - 1].push_back(tbuf[x][i]);
                    }
                    tbuf.erase(tbuf.begin() + x);
                    x -= 1;
                    if (tbuf[x].empty())
                        y = 0;
                    else
                        y = tbuf[x].size() - 1;
                }
                else if (y > 0)
                {
                    remove(tbuf[x], y - 1);
                    y -= 1;

                    move_to_text(buf, x, 0);

                    int cur_page_y = y / (config._width - 1 - max_line_size);

                    if (cur_page_y != last_page_y)
                    {
                        last_page_y = cur_page_y;
                        break;
                    }

                    output(tbuf[x], get_min_y(cur_page_y), std::min(tbuf[x].size(), get_max_y(cur_page_y)));

                    for (int i = tbuf[x].size(); i < get_max_y(cur_page_y); i++)
                    {
                        std::wcout << ' ';
                    }

                    std::wcout.flush();

                    update_buttom(buf);

                    return;
                }
                break;
            case TAB: // TAB
                add(buf, 32);
                add(buf, 32);
                add(buf, 32);
                add(buf, 32);
                return;
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
        case UP: // UP
            if (input_command)
                return;

            if (x > 0)
            {
                x -= 1;

                if (tbuf[x].size() == 0)
                    y = 0;
                else
                    y = min(y, tbuf[x].size());
            }
            if (x / (config._height - 2) == last_page_x)
            {
                update_buttom(buf);
                return;
            }
            break;
        case DOWN: // DOWN
            if (input_command)
                return;

            if (x < (tbuf.size() - 1))
            {
                x += 1;

                if (tbuf[x].size() == 0)
                    y = 0;
                else
                    y = min(y, tbuf[x].size());
            }
            if (x / (config._height - 2) == last_page_x)
            {
                update_buttom(buf);
                return;
            }
            break;
        case LEFT: // LEFT
            if (input_command && command_cur_y != 0)
                command_cur_y--;

            if (y > 0)
                y -= 1;

            if (y / (config._width - 1 - max_line_size) == last_page_y)
            {
                update_buttom(buf);
                return;
            }
            break;
        case RIGHT: // RIGHT
            if (input_command && command_cur_y < command_inputed.size())
                command_cur_y++;

            if (y <= (tbuf[x].size() - 1) && tbuf[x].size() != 0)
            {
                y += 1;
            }

            if (y / (config._width - 1 - max_line_size) == last_page_y)
            {
                update_buttom(buf);
                return;
            }
            break;
        case DEL: // DEL
            if (y < tbuf[x].size() - 1)
            {
                remove(tbuf[x], y);
            }
            break;
        default: // 不关注的特殊键 无需处理
            break;
        }
    }

    print(buf);
    move_to_text(buf, x, y);
}

/*
重新设置配置内控制台大小
*/
void reset_console_size()
{
    // 获取宽高
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    config._width = csbi.dwSize.X;
    config._height = csbi.dwSize.Y;
}

/*
初始化
*/
void init()
{
    // 关闭io同步减少渲染时间
    std::ios::sync_with_stdio(0);
    std::wios::sync_with_stdio(0);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);
    std::wcin.tie(nullptr);
    std::wcout.tie(nullptr);
    std::setlocale(LC_ALL, "chs");

    // 获取屏幕宽高
    reset_console_size();

    // 初始化命令工厂
    command::g_command_factory.init();
}

/*
程序的主入口
*/
int exit_function main(int argc, const char **argv)
{
    // 初始化
    init();

    // 解析参数
    parse_args(argc, argv, config);

    // 初始清空屏幕
    cls();

    // 创建buffer
    Buffer buf("New Buffer");

    // 第一次渲染输出。循环内渲染和输出是惰性的。第一次不输出可能会导致一段时间内没有文字
    Renderer::rend(buf.getbuffer());
    print(buf);

    context.add_with_id(std::string("buffer"), &buf);
    context.add_with_id("should_exit", &should_exit);
    context.add_with_id("config", &config);

    // 开始主循环 主循环每次调用tick函数 直到通过命令退出
    while (!should_exit)
    {
        // 调用tick函数
        tick_loop(buf);
    }

    // 清空屏幕
    cls();

    return 0;
}