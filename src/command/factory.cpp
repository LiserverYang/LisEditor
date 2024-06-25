#include "factory.h"

void command_parser(std::wstring &command)
{
    std::vector<std::wstring> command_list = split(command);

    if (!command_list.empty())
        for (int i = 0; i < command::g_command_factory.list.size(); i++)
        {
            if (command::g_command_factory.list[i]->check(command_list[0]))
            {
                command::g_command_factory.list[i]->on_called(context, command_list);

                break;
            }
        }
}

void command::factory::regist_command(std::shared_ptr<command> com)
{
    for (int i = 0; i < list.size(); i++)
    {
        if (list[i]->command_id() == com->command_id())
        {
            return;
        }
    }

    list.push_back(com);
}

namespace command
{
    factory g_command_factory;
}