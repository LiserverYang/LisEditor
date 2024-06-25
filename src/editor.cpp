/*
 * 这是一个简易的文本编辑器 使用WindowsAPI、STL标准模板库
 * MIT 开源协议
 *
 * 额外的编译参数 -std=c++11 -Werror
 *
 * C++版本：c++11或以上
 * O2优化应不影响程序正常运行
 *
 * @licence MIT
 * @anchor Liserver-Yang
 */

#ifndef __cplusplus
#error This is a c++ project.
#endif

#include "config/config.h"
#include "renderer/renderer.h"
#include "platform/FileIO.h"
#include "command/default_command.h"

#include "core/buffer.h"
#include "platform/console.h"
#include "platform/input.h"

#include "core/editor_core.h"

void command::factory::init()
{
    regist_command(std::make_shared<command_save>());
    regist_command(std::make_shared<command_quit>());
    regist_command(std::make_shared<command_save_quit>());
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

    // 添加上下文
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