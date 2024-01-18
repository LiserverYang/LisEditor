# LisEditor

[English](./README.md)

这是一个简易的开源代码编辑器仓库。

## 构建

- Windows:
    ```shell
    .\build.cmd
    ```
    或
    ```shell
    g++ .\editor.cpp -std=c++11 -Werror -o liseditor
    ```

- MacOS or Linux:
    它目前无法在MacOS或Linux上使用。但是你仍然可以通过wine来间接地使用它。

## 如何使用

**./liseditor** **[文件名]** **-ro[只读]**

- 按下快捷键 **CTRL + S** 来保存更改
- 按下快捷键 **CTRL + C** 来退出程序。