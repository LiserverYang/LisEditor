#ifndef __BUFFER_H__
#define __BUFFER_H__

#include <vector>

#include "../platform/FileIO.h"
#include "../renderer/renderer.h"

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
    Buffer(str name = "New Buffer") : _name(name), _io(config._path) { sync(); }

    /*
    从[FileIO]中同步文本到缓冲区
    注意：当同步进行后，任何未保存的编辑都将被舍弃
    */
    void sync() noexcept;

    /*
    从[FileIO]中保存文本
    */
    void save() & noexcept;

    inline std::vector<WCharList> &getbuffer() { return text_buffer; }
    inline const str getname() { return _name; }
    inline int &getx() { return cur_x; }
    inline int &gety() { return cur_y; }
};

#endif // __BUFFER_H__