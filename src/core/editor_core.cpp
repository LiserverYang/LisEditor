#include "editor_core.h"

bool input_command = false;
std::wstring command_inputed = L"";
int command_cur_y = 0;
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

inline void update_buttom(Buffer &buf, bool move_mouse = true)
{
    if (move_mouse)
    {
        move_mouse_to(GetStdHandle(STD_OUTPUT_HANDLE), 0, config._height - 1);
    }

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
                    y = std::min((std::size_t)y, tbuf[x].size());
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
                    y = std::min((std::size_t)y, tbuf[x].size());
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