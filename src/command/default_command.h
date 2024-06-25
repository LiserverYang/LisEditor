
#ifndef __DEFAULT_COMMAND_H__
#define __DEFAULT_COMMAND_H__

#include "../core/editor_core.h"
#include "factory.h"

class command_save : public command::command
{
public:
    virtual int command_id() override { return 1; }

    virtual void on_called(ContextManager &context, std::vector<std::wstring> &argments) override
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
    virtual int command_id() override { return 2; }

    virtual void on_called(ContextManager &context, std::vector<std::wstring> &argments) override
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
    virtual int command_id() override { return 3; }

    virtual void on_called(ContextManager &context, std::vector<std::wstring> &argments) override
    {
        context.get_by_id<Buffer>("buffer")->save();
        *(context.get_by_id<bool>("should_exit")) = true;
    }

    virtual bool check(std::wstring command_name_str) override
    {
        return command_name_str == L"sq";
    }
};

class command_resize : public command::command
{
public:
    virtual int command_id() override { return 4; }

    virtual void on_called(ContextManager &context, std::vector<std::wstring> &argments) override
    {
        reset_console_size();
    }

    virtual bool check(std::wstring command_name_str) override
    {
        return command_name_str == L"resize";
    }
};

#endif // __DEFAULT_COMMAND_H__