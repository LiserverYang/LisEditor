#ifndef __EDITOR_CORE_H__
#define __EDITOR_CORE_H__

#include <cmath>

#include "buffer.h"
#include "../renderer/renderer.h"
#include "../core/definetion.h"
#include "../platform/input.h"
#include "../platform/console.h"
#include "../command/factory.h"

// 是否处于命令模式
extern bool input_command;
// 输入的命令
extern std::wstring command_inputed;
// 当前指向的y
extern int command_cur_y;

extern int last_page_x;
extern int last_page_y;

inline std::size_t get_max_y(int page_y);
inline std::size_t get_min_y(int page_y);

/*
格式化输出数字，按照宽度
*/
inline void wint_opt(int size, std::wint_t value);

/*
移动光标
*/
void move_mouse_to(HANDLE handle, int x, int y);
void move_to_command_text();

/*
移动到某个字符
*/
void move_to_text(Buffer &buf, int x, int y);

/*
更新底部
*/
inline void update_buttom(Buffer &buf, bool move_mouse);

/*
Buffer的非成员渲染函数
*/
void print(Buffer &buf);

/*
添加一个文字
*/
inline void key_function add(Buffer &buf, int key);

/*
编辑器逻辑的主体
*/
void exit_function tick_loop(Buffer &buf);

#endif