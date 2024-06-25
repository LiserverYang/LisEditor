#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "../core/definetion.h"

#include <string>
#include <cstring>

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
extern Configtion config;

/*
解析程序的所有参数
*/
void parse_args(int argc, const char **argv, Configtion &config) noexcept;

#endif