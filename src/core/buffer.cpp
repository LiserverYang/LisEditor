#include "buffer.h"
#include "../renderer/renderer.h"

void Buffer::sync() noexcept
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

void Buffer::save() & noexcept
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