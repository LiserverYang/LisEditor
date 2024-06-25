#pragma once

#ifndef __FACTORY_H__
#define __FACTORY_H__

#include <memory>
#include <vector>
#include <string>

#include "../context/context.h"
#include "../core/definetion.h"

namespace command
{
    class command
    {
    public:
        virtual void on_called(ContextManager& context, std::vector<std::wstring> &argments) {}
        virtual bool check(std::wstring command_name_str) {return true;}
        virtual int command_id() {return 0;}
    };

    class factory
    {
    public:
        std::vector<std::shared_ptr<command>> list;

        void init();

        void regist_command(std::shared_ptr<command> com);
    };

    extern factory g_command_factory;
};

/*
解析命令
*/
void command_parser(std::wstring &command);

#endif